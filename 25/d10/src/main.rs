use common::read_from_file;
use std::collections::HashSet;
use z3::ast::Int;
use z3::{Optimize, SatResult};

struct IndicatorMachine {
    target: usize,
    buttons: Vec<usize>,
}

impl IndicatorMachine {
    fn from_line(line: &str) -> Self {
        let (target_str, remaining) = line.split_once(' ').expect("Bad formatting!");

        let target_str = target_str.trim_matches(['[', ']']);

        let mut target = 0usize;

        for (index, character) in target_str.chars().enumerate() {
            if character == '#' {
                target += 1usize << index;
            }
        }

        let (buttons_str, _) = remaining.split_once(" {").expect("Bad formatting!");

        let mut buttons = vec![];
        for button_str in buttons_str.split(' ') {
            let button_str = button_str.trim_matches(['(', ')']);

            let mut button_val = 0usize;
            for index_str in button_str.split(',') {
                let index = index_str.parse::<usize>().expect("Bad formatting!");

                button_val += 1 << index;
            }
            buttons.push(button_val);
        }

        Self { target, buttons }
    }

    fn fewest_button_presses_for_indicator(&self) -> usize {
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

                    if next_element
                        .iter()
                        .fold(0, |acc, &val| self.buttons[val] ^ acc)
                        == self.target
                    {
                        return num_presses_this_round;
                    }
                    new_buttons.insert(next_element);
                }
            }
            buttons = new_buttons;
        }

        panic!("Shouldn't reach here!")
    }
}

struct JoltageMachine {
    buttons_increment_vecs: Vec<Vec<usize>>,
    joltage: Vec<usize>,
}

impl JoltageMachine {
    fn from_line(line: &str) -> Self {
        let (_, remaining) = line.split_once(' ').expect("Bad formatting!");

        let (buttons_str, remaining) = remaining.split_once(" {").expect("Bad formatting!");

        let mut buttons = vec![];
        for button_str in buttons_str.split(' ') {
            let button_str = button_str.trim_matches(['(', ')']);

            buttons.push(
                button_str
                    .split(',')
                    .map(|b_str| b_str.parse::<usize>().expect("Bad formatting!"))
                    .collect::<Vec<usize>>(),
            );
        }

        let joltage_str = remaining.trim_matches(['{', '}']);

        let joltage = joltage_str
            .split(',')
            .map(|joltage_str| joltage_str.parse::<usize>().expect("Bad formatting!"))
            .collect::<Vec<usize>>();

        let buttons_increment_vecs = buttons
            .into_iter()
            .map(|button| Self::increment_vec(joltage.len(), &button))
            .collect::<Vec<Vec<usize>>>();

        Self {
            buttons_increment_vecs,
            joltage,
        }
    }

    fn increment_vec(len: usize, entry: &Vec<usize>) -> Vec<usize> {
        let mut out = vec![0usize; len];
        for button in entry {
            out[*button] = 1;
        }
        out
    }

    fn fewest_button_presses_for_joltage(&self) -> usize {
        // Linear equation to see fewest button presses to equal
        // exactly the joltage counts

        let optimizer = Optimize::new();

        let mut button_names = vec![];
        let mut button_ints = vec![];

        for button_index in 0..self.buttons_increment_vecs.len() {
            button_names.push(format!("button_{button_index}").to_string());
            button_ints.push(Int::fresh_const(&button_names[button_index]));
            optimizer.assert(&button_ints[button_index].ge(0));
        }

        let mut button_equations = vec![vec![]; self.joltage.len()];

        for (button_index, button_increment_vec) in self.buttons_increment_vecs.iter().enumerate() {
            for (joltage_index, button_enabled) in button_increment_vec.iter().enumerate() {
                if *button_enabled > 0 {
                    button_equations[joltage_index].push(button_ints[button_index].clone());
                }
            }
        }

        let equations = button_equations
            .into_iter()
            .map(|button_equation| {
                button_equation
                    .into_iter()
                    .reduce(|a, b| a.clone() + b.clone())
                    .expect("Must be something")
            })
            .collect::<Vec<_>>();

        for (joltage_index, equation) in equations.iter().enumerate() {
            optimizer.assert(&equation.eq(self.joltage[joltage_index] as u64));
        }

        let mut output = 0u64;

        optimizer.minimize(&Int::add(&button_ints));

        match optimizer.check(&[]) {
            SatResult::Sat => {
                let model = optimizer.get_model().expect("Must be here");
                for button in &button_ints {
                    output += model
                        .eval(button, true)
                        .expect("Must be here")
                        .as_u64()
                        .expect("gotta work");
                }
            }
            _ => panic!("Panic!"),
        }

        output as usize
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let indicator_machines = raw_lines
        .iter()
        .map(|line| IndicatorMachine::from_line(line))
        .collect::<Vec<IndicatorMachine>>();

    let p1_solution = indicator_machines
        .iter()
        .map(|indicator_machine| indicator_machine.fewest_button_presses_for_indicator())
        .sum::<usize>();

    println!("P1 Solution: {p1_solution}");

    let joltage_machines = raw_lines
        .iter()
        .map(|line| JoltageMachine::from_line(line))
        .collect::<Vec<JoltageMachine>>();

    let p2_solution = joltage_machines
        .iter()
        .map(|joltage_machine| joltage_machine.fewest_button_presses_for_joltage())
        .sum::<usize>();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}
