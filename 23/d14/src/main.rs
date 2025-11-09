use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

type Tile = char;

const MOVEABLE_STONE: Tile = 'O';
const EMPTY_TILE: Tile = '.';
const IMMOVEABLE_STONE: Tile = '#';

struct Field {
    field: Vec<Vec<Tile>>,
}

impl Field {
    fn from_raw_lines(raw_lines: RawLines) -> Self {
        Self {
            field: raw_lines
                .into_iter()
                .map(|line| line.chars().collect::<Vec<Tile>>())
                .collect::<Vec<Vec<Tile>>>(),
        }
    }

    fn print_size(&self) {
        println!("Field: {} x {}", self.field.len(), self.field[0].len());
    }

    fn print_field(&self) {
        for row in &self.field {
            for tile in row {
                print!("{tile}");
            }
            println!();
        }
    }

    fn roll_north(&self) -> Self {
        let mut field = self.field.clone();

        for row_index in 0..field.len() {
            let row = field[row_index].clone();

            for col_index in 0..row.len() {
                let tile = &row[col_index];

                let mut cur_row_index = row_index;

                if *tile == MOVEABLE_STONE {
                    while cur_row_index > 0
                        && Field::can_move(&field, (cur_row_index - 1, col_index))
                    {
                        field[cur_row_index][col_index] = EMPTY_TILE;
                        cur_row_index -= 1;
                        field[cur_row_index][col_index] = MOVEABLE_STONE;
                    }
                }
            }
        }
        Self { field }
    }

    fn can_move(field: &Vec<Vec<Tile>>, coordinates: (usize, usize)) -> bool {
        let (row_index, col_index) = coordinates;

        field[row_index][col_index] == EMPTY_TILE
    }

    fn calculate_load(&self) -> usize {
        let mut load = 0;
        for (row_index, row) in self.field.iter().enumerate() {
            for tile in row {
                if *tile == MOVEABLE_STONE {
                    load += self.field.len() - row_index;
                }
            }
        }
        load
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let field = Field::from_raw_lines(raw_lines);

    let field = field.roll_north();

    println!("P1 Solution: {}", field.calculate_load());

    Ok(())
}
