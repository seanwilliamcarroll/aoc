use common::{Grid, RawLines, Tile, get_input_file, grid_from_raw_lines, read_from_file};
use std::collections::HashMap;

const LASER_LINE: Tile = '|';

const SPLITTER: Tile = '^';

const START_TILE: Tile = 'S';

struct TachyonManifold {
    grid: Grid,
    current_row: usize,
    columns: HashMap<usize, usize>,
    num_beam_splits: usize,
}

impl TachyonManifold {
    fn from_raw_lines(lines: RawLines) -> Self {
        let grid = grid_from_raw_lines(lines);

        let starting_column = grid[0]
            .iter()
            .position(|&tile| tile == START_TILE)
            .expect("Required!");

        let current_row = 0usize;

        let columns: HashMap<usize, usize> = HashMap::from([(starting_column, 1)]);

        Self {
            grid,
            current_row,
            columns,
            num_beam_splits: 0usize,
        }
    }

    fn simulate_one_row(&mut self) {
        let row_index = self.current_row;

        let mut new_columns: HashMap<usize, usize> = HashMap::new();

        let add_or_initialize =
            |hashmap: &mut HashMap<usize, usize>, index: usize, increment: usize| {
                if let Some(value) = hashmap.get_mut(&index) {
                    *value += increment;
                } else {
                    hashmap.insert(index, increment);
                }
            };

        for (col_index, num_beams) in &self.columns {
            let col_index = *col_index;
            let num_beams = *num_beams;
            let tile = &self.grid[row_index][col_index];
            if *tile == SPLITTER {
                // Assuming that we don't go off the grid after splitting
                self.grid[row_index][col_index - 1] = LASER_LINE;
                add_or_initialize(&mut new_columns, col_index - 1, num_beams);
                self.grid[row_index][col_index + 1] = LASER_LINE;
                add_or_initialize(&mut new_columns, col_index + 1, num_beams);
                self.num_beam_splits += 1;
            } else {
                self.grid[row_index][col_index] = LASER_LINE;
                add_or_initialize(&mut new_columns, col_index, num_beams);
            }
        }

        self.columns = new_columns;

        self.current_row += 1;
    }

    fn simulate(&mut self) {
        while self.current_row < self.grid.len() {
            self.simulate_one_row();
        }
    }

    fn get_num_timelines(&self) -> usize {
        self.columns.values().sum::<usize>()
    }
}

fn main() -> std::io::Result<()> {
    let filepath = get_input_file()?;
    let raw_lines = read_from_file(&filepath)?;

    println!("Found {} lines", raw_lines.len());

    let mut tm = TachyonManifold::from_raw_lines(raw_lines);

    println!("{} x {} grid", tm.grid.len(), tm.grid[0].len());

    tm.simulate();

    let p1_solution = tm.num_beam_splits;

    println!("P1 Solution: {p1_solution}");

    let p2_solution = tm.get_num_timelines();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}
