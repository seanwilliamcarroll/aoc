use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Tile = char;

#[derive(Clone)]
struct LavaField {
    field: Vec<Vec<Tile>>,
}

impl LavaField {
    fn from_raw_lines(lines: RawLines) -> Self {
        let field = lines
            .into_iter()
            .map(|line| line.chars().collect::<Vec<Tile>>())
            .collect::<Vec<Vec<Tile>>>();

        assert!(field.len() % 2 == 1);
        assert!(field[0].len() % 2 == 1);

        Self { field }
    }

    fn print_size(&self) {
        println!("{} x {} Field", self.field.len(), self.field[0].len());
    }

    fn print_field(&self) {
        for line in &self.field {
            for tile in line {
                print!("{tile}");
            }
            println!();
        }
    }

    fn transpose(&self) -> Self {
        let mut new_field: Vec<Vec<Tile>> = vec![vec![]; self.field[0].len()];

        for row in &self.field {
            for (col_index, tile) in row.iter().enumerate() {
                new_field[col_index].push(*tile);
            }
        }
        Self { field: new_field }
    }

    fn flip(&self) -> Self {
        let mut new_field: Vec<Vec<Tile>> = vec![vec![]; self.field.len()];
        for (row_index, row) in self.field.iter().enumerate() {
            for tile in row {
                new_field[self.field.len() - 1 - row_index].push(*tile);
            }
        }
        Self { field: new_field }
    }

    fn find_horizontal_mirror_from_top_with_smudge(
        &self,
        max_incorrect_in_a_row: usize,
        number_incorrect_rows: usize,
    ) -> Option<usize> {
        // Try find row to match either first row or last row
        // This is required since the reflection has to reach one end or the other

        let length = self.field.len();

        let first_row = &self.field[0];
        let end_half_index = (length - 1) / 2;
        assert!((end_half_index * 2 + 1) == length);

        let number_mismatched = |row_a: &Vec<Tile>, row_b: &Vec<Tile>| -> usize {
            let mut count = 0;
            for (elem_a, elem_b) in std::iter::zip(row_a, row_b) {
                if elem_a != elem_b {
                    count += 1;
                }
                if count > max_incorrect_in_a_row {
                    return count;
                }
            }
            count
        };

        let mut end_rows = vec![];
        for row_index in 0..end_half_index {
            let row_index = row_index * 2 + 1;
            if number_mismatched(first_row, &self.field[row_index]) <= max_incorrect_in_a_row {
                end_rows.push(row_index);
            }
        }

        'outer: for end_row in &end_rows {
            let mut total_num_mismatch = 0;
            let reflecting_rows = (end_row + 1) / 2;

            for index in 0..reflecting_rows {
                let num_mismatched =
                    number_mismatched(&self.field[index], &self.field[end_row - index]);
                if num_mismatched > max_incorrect_in_a_row {
                    continue 'outer;
                }
                if max_incorrect_in_a_row > 0 && num_mismatched == max_incorrect_in_a_row {
                    if total_num_mismatch == number_incorrect_rows {
                        continue 'outer;
                    }
                    total_num_mismatch += 1;
                }
            }
            if total_num_mismatch == number_incorrect_rows {
                return Some(reflecting_rows);
            }
        }
        None
    }

    fn find_horizontal_mirror(
        &self,
        max_incorrect_in_a_row: usize,
        number_incorrect_rows: usize,
    ) -> Option<usize> {
        if let Some(index) = self.find_horizontal_mirror_from_top_with_smudge(
            max_incorrect_in_a_row,
            number_incorrect_rows,
        ) {
            Some(index)
        } else if let Some(index) = self.flip().find_horizontal_mirror_from_top_with_smudge(
            max_incorrect_in_a_row,
            number_incorrect_rows,
        ) {
            Some(self.field.len() - index)
        } else {
            None
        }
    }
}

fn fields_from_raw_lines(lines: RawLines) -> Vec<LavaField> {
    let mut field_lines: RawLines = vec![];

    let mut all_field_lines: Vec<RawLines> = vec![];

    for line in lines {
        if line.is_empty() {
            all_field_lines.push(field_lines);
            field_lines = vec![];
        } else {
            field_lines.push(line);
        }
    }
    if !field_lines.is_empty() {
        all_field_lines.push(field_lines);
    }

    all_field_lines
        .into_iter()
        .map(|lines| LavaField::from_raw_lines(lines))
        .collect::<Vec<LavaField>>()
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let lava_fields = fields_from_raw_lines(raw_lines);

    println!("Found {} fields", lava_fields.len());

    let mut p1_solution = 0usize;

    for lava_field in &lava_fields {
        if let Some(row_index) = lava_field.find_horizontal_mirror(0, 0) {
            p1_solution += row_index * 100;
            continue;
        }

        let transposed = lava_field.transpose();
        if let Some(col_index) = transposed.find_horizontal_mirror(0, 0) {
            p1_solution += col_index;
            continue;
        }
        panic!("Unable to find reflection!!");
    }

    println!("P1 Solution: {p1_solution}");

    let mut p2_solution = 0usize;

    for lava_field in &lava_fields {
        if let Some(row_index) = lava_field.find_horizontal_mirror(1, 1) {
            p2_solution += row_index * 100;
            continue;
        }

        let transposed = lava_field.transpose();
        if let Some(col_index) = transposed.find_horizontal_mirror(1, 1) {
            p2_solution += col_index;
            continue;
        }
        panic!("Unable to find reflection!!");
    }

    println!("P2 Solution: {p2_solution}");

    Ok(())
}
