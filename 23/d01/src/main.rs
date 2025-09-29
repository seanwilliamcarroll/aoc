use std::collections::HashMap;
use std::fs::File;
use std::io::{self, BufRead};

fn get_line_value_p1(line: &str) -> u32 {
    let mut output: u32 = 0;
    let mut first = true;
    let mut last: u32 = 0;
    for character in line.chars() {
        if character.is_digit(10) {
            last = character.to_digit(10).expect("Unexpected");
            if first {
                output = last * 10;
                first = false;
            }
        }
    }
    output + last
}

fn get_line_value_p2(line: &str) -> u32 {
    assert!(line.is_ascii());
    let digit_map = HashMap::from([
        ("one", 1),
        ("two", 2),
        ("three", 3),
        ("four", 4),
        ("five", 5),
        ("six", 6),
        ("seven", 7),
        ("eight", 8),
        ("nine", 9),
    ]);

    let mut output: u32 = 0;
    let mut first = true;
    let mut last: u32 = 0;
    for (index, character) in line.char_indices() {
        if character.is_digit(10) {
            last = character.to_digit(10).expect("Just checked this");
            if first {
                output = last * 10;
                first = false;
            }
            continue;
        }
        for (full_string, value) in &digit_map {
            let full_string = *full_string;
            let string_len = full_string.len();
            if string_len + index > line.len() {
                continue;
            }
            if line[index..index + string_len] == *full_string {
                last = *value;
                if first {
                    output = last * 10;
                    first = false;
                }
            }
        }
    }

    output + last
}

fn main() -> std::io::Result<()> {
    let file = File::open("input.txt")?;

    let reader = io::BufReader::new(file);

    let mut sum_p1 = 0u32;
    let mut sum_p2 = 0u32;
    for line in reader.lines() {
        let line = line?;
        sum_p1 += get_line_value_p1(&line.clone());
        sum_p2 += get_line_value_p2(&line);
    }
    println!("P1: {}", sum_p1);
    println!("P2: {}", sum_p2);
    Ok(())
}
