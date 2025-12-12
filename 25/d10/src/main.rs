use std::collections::HashSet;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

struct Machine {
    target: usize,
    buttons: Vec<usize>,
    _joltage: Vec<usize>,
}

impl Machine {
    fn from_line(line: &str) -> Self {
        let (target_str, remaining) = line.split_once(' ').expect("Bad formatting!");

        let target_str = target_str.trim_matches(&['[', ']']);

        let mut target = 0usize;

        for (index, character) in target_str.chars().enumerate() {
            match character {
                '#' => target += 1usize << index,
                _ => (),
            }
        }

        let (buttons_str, remaining) = remaining.split_once(" {").expect("Bad formatting!");

        let mut buttons = vec![];
        for button_str in buttons_str.split(' ') {
            let button_str = button_str.trim_matches(&['(', ')']);

            let mut button_val = 0usize;
            for index_str in button_str.split(',') {
                let index = index_str.parse::<usize>().expect("Bad formatting!");

                button_val += 1 << index;
            }
            buttons.push(button_val);
        }

        let joltage_str = remaining.trim_matches(&['{', '}']);

        let joltage = joltage_str
            .split(',')
            .map(|joltage_str| joltage_str.parse::<usize>().expect("Bad formatting!"))
            .collect::<Vec<usize>>();

        Self {
            target,
            buttons,
            _joltage: joltage,
        }
    }

    fn fewest_button_presses(&self) -> usize {
        // basically going to try 1 at a time, 2 at a time, ...

        let mut buttons: HashSet<Vec<usize>> = HashSet::new();

        // Trivial case of one button being pressed

        for index in 0..self.buttons.len() {
            if self.target == self.buttons[index] {
                return 1;
            }
            buttons.insert(vec![index]);
        }

        for num_presses_this_round in 2..=self.buttons.len() {
            // Generate combo
            // For each element

            let mut new_buttons: HashSet<Vec<usize>> = HashSet::new();
            // Iterate over the existing elements in the hashset,
            // add index that does not yet appear
            for element in buttons.into_iter() {
                let largest_index = element
                    .last()
                    .expect("We know they all are at least 1 in length");

                for next_index in (largest_index + 1)..self.buttons.len() {
                    let mut next_element = element.clone();
                    next_element.push(next_index);

                    // println!("Try combo: {next_element:?}");

                    if next_element
                        .iter()
                        .fold(0, |acc, &val| self.buttons[val] ^ acc)
                        == self.target
                    {
                        // println!("valid: {next_element:?}");
                        return num_presses_this_round;
                    }
                    new_buttons.insert(next_element);
                }
            }
            buttons = new_buttons;
        }

        0usize
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let machines = raw_lines
        .iter()
        .map(|line| Machine::from_line(line))
        .collect::<Vec<Machine>>();

    // for machine in &machines {
    //     print!("{:b} | ", machine.target);

    //     for button in &machine.buttons {
    //         print!("{:b}, ", button);
    //     }
    //     println!();
    //     println!("{} | {:?}", machine.target, machine.buttons);

    //     let fewest = machine.fewest_button_presses();

    //     println!("fewest: {fewest}");

    //     // break;
    // }

    let p1_solution = machines
        .iter()
        .map(|machine| machine.fewest_button_presses())
        .sum::<usize>();

    println!("P1 Solution: {p1_solution}");

    Ok(())
}
