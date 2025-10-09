use std::collections::HashMap;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

const NODE_NAME_LENGTH: usize = 3;
const START_NODE_CHAR: char = 'A';
const END_NODE_CHAR: char = 'Z';

type NodeName = [char; NODE_NAME_LENGTH];

const START_NODE_NAME: NodeName = [START_NODE_CHAR; NODE_NAME_LENGTH];
const END_NODE_NAME: NodeName = [END_NODE_CHAR; NODE_NAME_LENGTH];

fn to_node_name(chars: &str) -> NodeName {
    chars
        .chars()
        .collect::<Vec<char>>()
        .try_into()
        .expect("Must be {NODE_NAME_LENGTH} characters")
}

fn is_start_node(node_name: &NodeName) -> bool {
    node_name[2] == START_NODE_CHAR
}

fn is_end_node(node_name: &NodeName) -> bool {
    node_name[2] == END_NODE_CHAR
}

struct Node {
    name: NodeName,
    left: NodeName,
    right: NodeName,
}

impl Node {
    fn from_line(line: &str) -> Self {
        let (name, connections) = line.split_once(" = ").expect("Bad formatting");

        let (left, right) = connections
            .trim_matches(|character| character == '(' || character == ')')
            .split_once(", ")
            .expect("Bad formatting");

        let name = to_node_name(name);
        let left = to_node_name(left);
        let right = to_node_name(right);

        Self { name, left, right }
    }
}

type Instructions = String;

struct Network {
    steps: Instructions,
    nodes: HashMap<NodeName, Node>,
}

impl Network {
    fn from_lines(lines: &RawLines) -> Self {
        let instructions = lines[0].to_string();

        let mut nodes = HashMap::new();

        for line in &lines[2..] {
            let node = Node::from_line(line);
            nodes.insert(node.name, node);
        }

        Self {
            steps: instructions,
            nodes,
        }
    }

    fn simulate(&self, start_node: &NodeName) -> (usize, NodeName) {
        // return number of steps to finish
        self.simulate_until(start_node, is_end_node)
    }

    fn simulate_until<F>(
        &self,
        start_node: &NodeName,
        mut is_end_condition_function: F,
    ) -> (usize, NodeName)
    where
        F: FnMut(&NodeName) -> bool,
    {
        // return number of steps to finish

        let mut current_node_name: NodeName = *start_node;

        let mut steps: usize = 0;

        while !is_end_condition_function(&current_node_name) {
            let current_node = self.nodes.get(&current_node_name).expect("Unknown name?");
            let next_instruction = self
                .steps
                .chars()
                .nth(steps % self.steps.len())
                .expect("Must be an index");

            let next_node_name = if next_instruction == 'L' {
                current_node.left
            } else if next_instruction == 'R' {
                current_node.right
            } else {
                panic!("Bad instruction");
            };

            let next_node = self.nodes.get(&next_node_name).expect("Must be a node");
            current_node_name = next_node.name;
            steps += 1;
        }

        (steps, current_node_name)
    }

    fn all_start_nodes(&self) -> Vec<NodeName> {
        self.nodes
            .values()
            .filter(|node| is_start_node(&node.name))
            .map(|node| node.name)
            .collect::<Vec<NodeName>>()
    }
}

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn gcd(input_a: usize, input_b: usize) -> usize {
    if input_b == 0 {
        input_a
    } else {
        gcd(input_b, input_a % input_b)
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} instructions", raw_lines.len());

    let network = Network::from_lines(&raw_lines);

    let (p1_steps, p1_end_node) = network.simulate(&START_NODE_NAME);
    assert!(p1_end_node == END_NODE_NAME);
    println!("P1 steps: {p1_steps}");

    let start_node_names: Vec<NodeName> = network.all_start_nodes();

    let mut all_steps = vec![];

    for start_node_name in &start_node_names {
        let (steps, _) = network.simulate(start_node_name);

        all_steps.push(steps);
    }

    let p2_output = all_steps
        .iter()
        .fold(1, |acc, &next| acc * next / gcd(acc, next));

    println!("P2 Output: {p2_output}");

    Ok(())
}
