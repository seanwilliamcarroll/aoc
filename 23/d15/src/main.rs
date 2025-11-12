use std::collections::HashMap;
use std::fs::File;
use std::io::{self, BufRead};
use std::sync::atomic::{AtomicUsize, Ordering};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

fn hash_algorithm(line: &str) -> usize {
    line.bytes()
        .fold(0usize, |acc, num| ((acc + num as usize) * 17) % 256)
}

type FocalLength = u8;

type Label = String;

enum Operation {
    Assignment(Label, FocalLength),
    Removal(Label),
}

impl Operation {
    fn from_raw_str(line: &str) -> Self {
        if let Some((label, value_string)) = line.split_once('=') {
            return Self::Assignment(
                label.to_string(),
                value_string
                    .parse::<FocalLength>()
                    .expect("Bad formatting!"),
            );
        }
        let mut label = line.to_string();
        if let Some('-') = label.pop() {
            return Self::Removal(label);
        }
        panic!("Bad formatting!!");
    }

    fn label(&self) -> &str {
        match self {
            Self::Assignment(label, _) => label,
            Self::Removal(label) => label,
        }
    }

    fn get_hash(&self) -> usize {
        hash_algorithm(self.label())
    }
}

#[derive(Clone)]
struct LensBoxEntry {
    focal_length: FocalLength,
    sequence_number: usize,
}

impl LensBoxEntry {
    fn next_sequence_number() -> usize {
        static CURRENT_SEQUENCE_NUMBER: AtomicUsize = AtomicUsize::new(0usize);

        let output = CURRENT_SEQUENCE_NUMBER.load(Ordering::Relaxed);
        CURRENT_SEQUENCE_NUMBER.fetch_add(1usize, Ordering::Relaxed);
        output
    }

    fn new(focal_length: FocalLength) -> Self {
        Self {
            focal_length,
            sequence_number: Self::next_sequence_number(),
        }
    }

    fn focusing_power(&self) -> usize {
        self.focal_length as usize
    }
}

struct LensBox {
    slots: HashMap<Label, LensBoxEntry>,
}

impl LensBox {
    fn new() -> Self {
        Self {
            slots: HashMap::new(),
        }
    }

    fn assign(&mut self, label: &str, focal_length: FocalLength) {
        if let Some(entry) = self.slots.get_mut(label) {
            entry.focal_length = focal_length;
        } else {
            self.slots
                .insert(label.to_string(), LensBoxEntry::new(focal_length));
        }
    }

    fn remove(&mut self, label: &str) {
        let _ = self.slots.remove(label);
    }

    fn focusing_power(&self) -> usize {
        let mut slots = self.slots.clone().into_values().collect::<Vec<_>>();
        slots.sort_by(|entry_a, entry_b| entry_a.sequence_number.cmp(&entry_b.sequence_number));

        let mut power = 0;
        for (index, entry) in slots.into_iter().enumerate() {
            power += (index + 1) * entry.focusing_power();
        }

        power
    }
}

struct HolidayHashMap {
    boxes: [LensBox; 256],
}

impl HolidayHashMap {
    fn new() -> Self {
        Self {
            boxes: std::array::from_fn(|_| LensBox::new()),
        }
    }

    fn apply_operation(&mut self, operation: &Operation) {
        let hash_value = operation.get_hash();

        let lens_box = &mut self.boxes[hash_value as usize];

        match operation {
            Operation::Assignment(label, focal_length) => {
                lens_box.assign(label, *focal_length);
            }
            Operation::Removal(label) => {
                lens_box.remove(label);
            }
        }
    }

    fn focusing_power(&self) -> usize {
        self.boxes
            .iter()
            .enumerate()
            .map(|(index, lens_box)| (index + 1) * lens_box.focusing_power())
            .sum::<usize>()
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let raw_line = &raw_lines[0];

    let p1_solution: usize = raw_line.split(',').map(hash_algorithm).sum();

    println!("P1 solution: {p1_solution}");

    let mut hash_map = HolidayHashMap::new();

    let operations = raw_line
        .split(',')
        .map(Operation::from_raw_str)
        .collect::<Vec<Operation>>();

    for operation in &operations {
        hash_map.apply_operation(operation);
    }

    println!("P2 solution: {}", hash_map.focusing_power());

    Ok(())
}
