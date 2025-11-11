use std::collections::HashSet;
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
// const IMMOVEABLE_STONE: Tile = '#';

#[derive(Clone)]
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
        println!(
            "================================================================================"
        );
        for row in &self.field {
            for tile in row {
                print!("{tile}");
            }
            println!();
        }
    }

    fn roll_north(&mut self) {
        for row_index in 0..self.field.len() {
            for col_index in 0..self.field[row_index].len() {
                let tile = self.field[row_index][col_index];

                let mut cur_row_index = row_index;

                if tile != MOVEABLE_STONE {
                    continue;
                }
                while cur_row_index > 0
                    && Field::can_move(&self.field, (cur_row_index - 1, col_index))
                {
                    self.field[cur_row_index][col_index] = EMPTY_TILE;
                    cur_row_index -= 1;
                    self.field[cur_row_index][col_index] = MOVEABLE_STONE;
                }
            }
        }
    }

    fn roll_west(&mut self) {
        for col_index in 0..self.field[0].len() {
            for row_index in 0..self.field.len() {
                let tile = self.field[row_index][col_index];

                let mut cur_col_index = col_index;

                if tile != MOVEABLE_STONE {
                    continue;
                }
                while cur_col_index > 0
                    && Field::can_move(&self.field, (row_index, cur_col_index - 1))
                {
                    self.field[row_index][cur_col_index] = EMPTY_TILE;
                    cur_col_index -= 1;
                    self.field[row_index][cur_col_index] = MOVEABLE_STONE;
                }
            }
        }
    }

    fn roll_south(&mut self) {
        for row_index in (0..self.field.len()).rev() {
            for col_index in 0..self.field[row_index].len() {
                let tile = self.field[row_index][col_index];

                let mut cur_row_index = row_index;

                if tile != MOVEABLE_STONE {
                    continue;
                }
                while cur_row_index < self.field.len() - 1
                    && Field::can_move(&self.field, (cur_row_index + 1, col_index))
                {
                    self.field[cur_row_index][col_index] = EMPTY_TILE;
                    cur_row_index += 1;
                    self.field[cur_row_index][col_index] = MOVEABLE_STONE;
                }
            }
        }
    }

    fn roll_east(&mut self) {
        for col_index in (0..self.field[0].len()).rev() {
            for row_index in 0..self.field.len() {
                let tile = self.field[row_index][col_index];

                let mut cur_col_index = col_index;

                if tile != MOVEABLE_STONE {
                    continue;
                }
                while cur_col_index < self.field[0].len() - 1
                    && Field::can_move(&self.field, (row_index, cur_col_index + 1))
                {
                    self.field[row_index][cur_col_index] = EMPTY_TILE;
                    cur_col_index += 1;
                    self.field[row_index][cur_col_index] = MOVEABLE_STONE;
                }
            }
        }
    }

    fn roll_one_cycle(&mut self) {
        self.roll_north();
        self.roll_west();
        self.roll_south();
        self.roll_east();
    }

    fn roll_1000000000_cycles(&mut self) {
        let mut seen_so_far: HashSet<Vec<Vec<Tile>>> = HashSet::new();

        self.roll_one_cycle();

        let mut num_cycles = 1;

        while seen_so_far.insert(self.field.clone()) {
            self.roll_one_cycle();
            num_cycles += 1;
        }

        let initial_rolls = num_cycles;

        let end_field = self.field.clone();

        self.roll_one_cycle();
        num_cycles = 1;
        while end_field != self.field {
            self.roll_one_cycle();
            num_cycles += 1;
        }

        num_cycles = (1000000000 - initial_rolls) % num_cycles;

        for _ in 0..num_cycles {
            self.roll_one_cycle()
        }
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

    let mut field = Field::from_raw_lines(raw_lines);

    let mut p1_field = field.clone();

    p1_field.roll_north();

    println!("P1 Solution: {}", p1_field.calculate_load());

    field.roll_1000000000_cycles();

    println!("P2 Solution: {}", field.calculate_load());

    Ok(())
}
