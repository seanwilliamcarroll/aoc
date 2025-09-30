use std::fs::File;
use std::io::{self, BufRead};

struct NumberPos {
    row_index: usize,
    col_index: usize,
    string_len: usize,
}

fn is_symbol(character: char) -> bool {
    !character.is_digit(10) && character != '.'
}

fn get_character_at(grid: &Vec<String>, row_index: usize, col_index: usize) -> char {
    println!("Check character at ({row_index}, {col_index})");
    grid[row_index]
        .chars()
        .nth(col_index)
        .expect("Already checked this")
}

fn main() -> std::io::Result<()> {
    let file = File::open("input.txt")?;

    let reader = io::BufReader::new(file);
    let mut schematic = Vec::<String>::new();
    for line in reader.lines() {
        let line = line?;
        assert!(line.is_ascii());
        schematic.push(line);
    }
    println!(
        "Schematic of size: {} x {}",
        schematic.len(),
        schematic[0].len()
    );

    let mut number_map = vec![];

    // Find all numbers and their positions
    for (row_index, row) in schematic.iter().enumerate() {
        let mut current_number = "".to_string();
        for (col_index, character) in row.chars().enumerate() {
            if character.is_digit(10) {
                current_number.push(character);
            } else if current_number.len() > 0 {
                // println!("Number: {}", current_number);
                number_map.push((
                    current_number
                        .parse::<u32>()
                        .expect("Just checked these digits"),
                    NumberPos {
                        row_index,
                        col_index: col_index - current_number.len(),
                        string_len: current_number.len(),
                    },
                ));
                current_number = "".to_string();
            }
        }
        if current_number.len() > 0 {
            // println!("Number: {}", current_number);
            number_map.push((
                current_number
                    .parse::<u32>()
                    .expect("Just checked these digits"),
                NumberPos {
                    row_index,
                    col_index: schematic[0].len() - current_number.len(),
                    string_len: current_number.len(),
                },
            ));
        }
    }

    let mut sum_p1 = 0;
    for (
        number,
        NumberPos {
            row_index,
            col_index,
            string_len,
        },
    ) in number_map
    {
        // println!("Number: {number} at ({row_index}, {col_index}) of length: {string_len} with character: {character}");
        let row_begin = if row_index == 0 {
            row_index
        } else {
            row_index - 1
        };
        let row_end = if row_index == schematic.len() - 1 {
            row_index
        } else {
            row_index + 1
        };
        let col_begin = if col_index == 0 {
            col_index
        } else {
            col_index - 1
        };
        let col_end = if col_index + string_len >= schematic[0].len() - 1 {
            schematic[0].len() - 1
        } else {
            col_index + string_len
        };
        let mut found = false;
        println!("Number: {number} at ({row_index}, {col_index}) of length: {string_len}");
        for row_iter in row_begin..row_end + 1 {
            for col_iter in col_begin..col_end + 1 {
                let character = get_character_at(&schematic, row_iter, col_iter);
                if is_symbol(character) {
                    println!("Found char: {character}");
                    sum_p1 += number;
                    found = true;
                }
                if found {
                    break;
                }
            }
            if found {
                break;
            }
        }
    }
    println!("Sum P1: {}", sum_p1);

    Ok(())
}
