use common::{RawLines, get_input_file, read_from_file};
use std::cell::RefCell;
use std::collections::HashMap;
use std::rc::Rc;

const START_NODE_LABEL: &str = "you";
const SERVER_NODE_LABEL: &str = "svr";
const END_NODE_LABEL: &str = "out";
const DAC_NODE_LABEL: &str = "dac";
const FFT_NODE_LABEL: &str = "fft";

type NodePtr = Rc<RefCell<Node>>;

struct Node {
    id: usize,
    paths: Vec<NodePtr>,
}

impl Node {
    fn new(id: usize) -> Self {
        Self { id, paths: vec![] }
    }

    fn add_output(&mut self, next: &NodePtr) {
        self.paths.push(Rc::clone(next));
    }
}

struct Graph {
    label_to_id: HashMap<String, usize>,
    id_to_node: HashMap<usize, NodePtr>,
    fft_id: usize,
    dac_id: usize,
}

impl Graph {
    fn from_raw_lines(lines: RawLines) -> Self {
        let label_to_outputs = lines
            .into_iter()
            .map(|line| {
                let (label, remaining) = line.split_once(':').expect("Bad formatting!");

                let outputs = remaining
                    .trim()
                    .split(' ')
                    .map(std::string::ToString::to_string)
                    .collect::<Vec<String>>();

                (label.to_string(), outputs)
            })
            .collect::<HashMap<String, Vec<String>>>();

        let mut label_to_node: HashMap<String, NodePtr> =
            HashMap::with_capacity(label_to_outputs.len());

        let mut id_to_node: HashMap<usize, NodePtr> =
            HashMap::with_capacity(label_to_outputs.len());

        let mut label_to_id: HashMap<String, usize> = HashMap::new();

        let mut id = 0usize;
        for label in label_to_outputs.keys() {
            let node = Rc::new(RefCell::new(Node::new(id)));
            label_to_node.insert(label.clone(), Rc::clone(&node));
            id_to_node.insert(id, node);
            label_to_id.insert(label.clone(), id);
            id += 1;
        }
        let end_node = Rc::new(RefCell::new(Node::new(id)));
        label_to_node.insert(END_NODE_LABEL.to_string(), Rc::clone(&end_node));
        id_to_node.insert(id, end_node);
        label_to_id.insert(END_NODE_LABEL.to_string(), id);

        for (label, outputs) in &label_to_outputs {
            let current_node = label_to_node.get(label).expect("Has to be in there");
            for output in outputs {
                let output_node = label_to_node.get(output).expect("Has to be in there");
                current_node.borrow_mut().add_output(output_node);
            }
        }

        let dac_id = *label_to_id.get(DAC_NODE_LABEL).expect("Must have dac");
        let fft_id = *label_to_id.get(FFT_NODE_LABEL).expect("Must have fft");

        Self {
            label_to_id,
            id_to_node,
            fft_id,
            dac_id,
        }
    }

    fn memoized_count_paths_between(
        &self,
        end_node_id: usize,
        id: usize,
        seen_fft: bool,
        seen_dac: bool,
        memoizer: &mut HashMap<(usize, bool, bool), usize>,
    ) -> usize {
        if id == end_node_id {
            return usize::from(seen_fft && seen_dac);
        }

        let seen_fft = seen_fft || id == self.fft_id;
        let seen_dac = seen_dac || id == self.dac_id;

        let next_node = self.id_to_node.get(&id).expect("Must have this node");

        let mut out = 0usize;

        for output_node in &next_node.borrow().paths {
            if let Some(found_paths) = memoizer.get(&(output_node.borrow().id, seen_fft, seen_dac))
            {
                out += found_paths;
            } else {
                let found_paths = self.memoized_count_paths_between(
                    end_node_id,
                    output_node.borrow().id,
                    seen_fft,
                    seen_dac,
                    memoizer,
                );
                out += found_paths;
                memoizer.insert((output_node.borrow().id, seen_fft, seen_dac), found_paths);
            }
        }
        out
    }

    fn count_paths_between(
        &self,
        start_label: String,
        end_label: String,
        need_fft_dac: bool,
    ) -> usize {
        let start_id = *self
            .label_to_id
            .get(&start_label)
            .expect("Couldn't find start_label!");

        let end_id = *self
            .label_to_id
            .get(&end_label)
            .expect("Couldn't find start_label!");

        self.memoized_count_paths_between(
            end_id,
            start_id,
            !need_fft_dac,
            !need_fft_dac,
            &mut HashMap::new(),
        )
    }

    fn count_paths_out_dac_fft(&self) -> usize {
        self.count_paths_between(
            SERVER_NODE_LABEL.to_string(),
            END_NODE_LABEL.to_string(),
            true,
        )
    }
}

fn main() -> std::io::Result<()> {
    let filepath = get_input_file()?;
    let raw_lines = read_from_file(&filepath)?;

    println!("Found {} lines", raw_lines.len());

    let graph = Graph::from_raw_lines(raw_lines);

    let p1_solution = graph.count_paths_between(
        START_NODE_LABEL.to_string(),
        END_NODE_LABEL.to_string(),
        false,
    );

    println!("P1 Solution: {p1_solution}");

    let p2_solution = graph.count_paths_out_dac_fft();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}
