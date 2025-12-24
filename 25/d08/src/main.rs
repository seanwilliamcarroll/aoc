use common::{RawLines, read_from_file};
use std::cell::RefCell;
use std::collections::{BinaryHeap, HashMap, HashSet};
use std::rc::Rc;

#[derive(Debug, Clone)]
struct Position {
    x: usize,
    y: usize,
    z: usize,
}

impl Position {
    fn from_line(line: &str) -> Self {
        let coords: Vec<&str> = line.split(',').collect();

        assert!(coords.len() == 3);

        let coords: Vec<usize> = coords
            .into_iter()
            .map(|val| val.parse::<usize>().expect("Formatting!"))
            .collect::<Vec<usize>>();

        Self {
            x: coords[0],
            y: coords[1],
            z: coords[2],
        }
    }

    fn squared_distance_apart(&self, other: &Self) -> usize {
        let exp: u32 = 2;

        ((self.x.abs_diff(other.x) as u64).pow(exp)
            + (self.y.abs_diff(other.y) as u64).pow(exp)
            + (self.z.abs_diff(other.z) as u64).pow(exp)) as usize
    }
}

#[derive(Clone)]
struct ConnectedGraph {
    vertices: HashSet<usize>,
    connections: HashSet<(usize, usize)>,
}

impl ConnectedGraph {
    fn from_vertex(position: usize) -> Self {
        Self {
            vertices: HashSet::from([position]),
            connections: HashSet::new(),
        }
    }

    fn add_edge(&mut self, position_0: usize, position_1: usize) {
        self.vertices.insert(position_0);
        self.vertices.insert(position_1);
        self.connections.insert((position_0, position_1));
        self.connections.insert((position_1, position_0));
    }

    fn contains_edge(&self, position_0: usize, position_1: usize) -> bool {
        self.connections.contains(&(position_0, position_1))
            || self.connections.contains(&(position_1, position_0))
    }

    fn from_connected_graphs(connected_graph_0: &mut Self, connected_graph_1: &mut Self) -> Self {
        Self {
            vertices: connected_graph_0
                .vertices
                .drain()
                .chain(connected_graph_1.vertices.drain())
                .collect::<HashSet<usize>>(),
            connections: connected_graph_0
                .connections
                .drain()
                .chain(connected_graph_1.connections.drain())
                .collect::<HashSet<(usize, usize)>>(),
        }
    }

    fn len(&self) -> usize {
        self.vertices.len()
    }
}

struct Graph {
    vertices: HashSet<usize>,
    sub_graphs: HashMap<usize, Rc<RefCell<ConnectedGraph>>>,
}

impl Graph {
    fn new() -> Self {
        Self {
            vertices: HashSet::new(),
            sub_graphs: HashMap::new(),
        }
    }

    fn add_vertex(&mut self, position: usize) {
        if self.vertices.insert(position) {
            self.sub_graphs.insert(
                position,
                Rc::new(RefCell::new(ConnectedGraph::from_vertex(position))),
            );
        }
    }

    fn add_edge(&mut self, position_0: usize, position_1: usize) -> Rc<RefCell<ConnectedGraph>> {
        self.add_vertex(position_0);
        self.add_vertex(position_1);

        let sub_graph_0 = self.sub_graphs.get(&position_0);
        let sub_graph_1 = self.sub_graphs.get(&position_1);

        let sub_graph_0 = sub_graph_0.expect("Just added");
        let sub_graph_1 = sub_graph_1.expect("Just added");

        if Rc::ptr_eq(sub_graph_0, sub_graph_1) {
            // Already the same graph
            sub_graph_0.borrow_mut().add_edge(position_0, position_1);
            Rc::clone(sub_graph_0)
        } else {
            // Merge
            let new_sub_graph = Rc::new(RefCell::new(ConnectedGraph::from_connected_graphs(
                &mut sub_graph_0.borrow_mut(),
                &mut sub_graph_1.borrow_mut(),
            )));

            for vertex in (*new_sub_graph).borrow().vertices.clone() {
                self.sub_graphs.remove(&vertex);
                self.sub_graphs.insert(vertex, Rc::clone(&new_sub_graph));
            }
            (*new_sub_graph)
                .borrow_mut()
                .add_edge(position_0, position_1);

            assert!(
                (*new_sub_graph)
                    .borrow()
                    .contains_edge(position_0, position_1)
            );
            new_sub_graph
        }
    }

    fn sub_graph_sizes(&self) -> HashSet<usize> {
        self.sub_graphs
            .values()
            .map(|sub_graph| (*sub_graph).borrow().len())
            .collect::<HashSet<usize>>()
    }
}

struct Boxes {
    positions: Vec<Position>,
    graph: Graph,
}

impl Boxes {
    fn from_raw_lines(lines: RawLines) -> Self {
        let positions = lines
            .into_iter()
            .map(|line| Position::from_line(&line))
            .collect::<Vec<Position>>();

        Self {
            positions,
            graph: Graph::new(),
        }
    }

    fn get_closest_pairs(&self) -> Vec<(usize, usize)> {
        let mut max_heap = BinaryHeap::new();

        let mut seen: HashSet<(usize, usize)> = HashSet::new();

        for position_0 in 0..self.positions.len() {
            for position_1 in 0..position_0 {
                if position_0 == position_1 {
                    continue;
                }

                if seen.contains(&(position_0, position_1))
                    || seen.contains(&(position_1, position_0))
                {
                    continue;
                }
                seen.insert((position_0, position_1));
                seen.insert((position_1, position_0));

                let squared_dist =
                    self.positions[position_0].squared_distance_apart(&self.positions[position_1]);

                max_heap.push((squared_dist, (position_0, position_1)));
            }
        }

        max_heap
            .into_sorted_vec()
            .into_iter()
            .map(|(_, entry)| entry)
            .collect::<Vec<(usize, usize)>>()
    }

    fn build_graphs_from_pairs_until_all_connected(
        &mut self,
        adjacency_list: Vec<(usize, usize)>,
    ) -> Option<(Position, Position)> {
        for entry in adjacency_list {
            let (pos_0, pos_1) = entry;
            let sub_graph = self.graph.add_edge(pos_0, pos_1);
            if (*sub_graph).borrow().len() == self.positions.len() {
                return Some((self.positions[pos_0].clone(), self.positions[pos_1].clone()));
            }
        }
        None
    }

    fn largest_n_circuits(&self, n: usize) -> Vec<usize> {
        let mut sizes = self
            .graph
            .sub_graph_sizes()
            .into_iter()
            .collect::<Vec<usize>>();

        // Can use sort_unstable for primitives like usize for faster perf,
        // with no sort issues
        sizes.sort_unstable();

        sizes.reverse();

        assert!(sizes.len() >= n);

        sizes.into_iter().take(n).collect::<Vec<usize>>()
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let mut boxes = Boxes::from_raw_lines(raw_lines.clone());

    println!("{} boxes", boxes.positions.len());

    let closest_pairs = boxes.get_closest_pairs();

    boxes.build_graphs_from_pairs_until_all_connected(
        closest_pairs
            .clone()
            .into_iter()
            .take(1000)
            .collect::<Vec<(usize, usize)>>(),
    );

    let largest_n_cir = boxes.largest_n_circuits(3);

    let p1_solution = largest_n_cir.into_iter().product::<usize>();

    println!("P1 Solution: {p1_solution}");

    boxes = Boxes::from_raw_lines(raw_lines);

    if let Some((position_0, position_1)) =
        boxes.build_graphs_from_pairs_until_all_connected(closest_pairs)
    {
        let p2_solution = position_0.x * position_1.x;

        println!("P2 Solution: {p2_solution}");
    }

    Ok(())
}
