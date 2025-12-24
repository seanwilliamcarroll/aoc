use common::{RawLines, read_from_file};

type Unit = usize;

enum Operation {
    Addition,
    Multiplication,
}

impl Operation {
    fn from_char(character: char) -> Self {
        match character {
            '+' => Self::Addition,
            '*' => Self::Multiplication,
            _ => panic!("Unknown character!"),
        }
    }
}

struct MathProblem {
    nums: Vec<Unit>,
    op: Operation,
}

impl MathProblem {
    fn from_raw_lines(lines: RawLines) -> Vec<Self> {
        let mut problems: Vec<Self> = vec![];

        let mut all_nums: Vec<Vec<Unit>> = vec![vec![]; lines.len() - 1];
        let mut all_operations: Vec<Operation> = vec![];

        for (index, line) in lines.into_iter().enumerate() {
            let tokens = line.split_whitespace();

            for token in tokens.into_iter() {
                if let Ok(value) = token.parse::<Unit>() {
                    all_nums[index].push(value);
                } else {
                    all_operations.push(Operation::from_char(
                        token.chars().next().expect("Bad formatting!"),
                    ));
                }
            }
        }

        let mut num_rows: Vec<Vec<Unit>> = vec![vec![]; all_operations.len()];

        for row in all_nums.into_iter() {
            for (index, value) in row.into_iter().enumerate() {
                num_rows[index].push(value);
            }
        }

        for (nums, op) in num_rows.into_iter().zip(all_operations.into_iter()) {
            problems.push(Self { nums, op })
        }

        problems
    }

    fn solve(&self) -> Unit {
        let operation = match self.op {
            Operation::Addition => |x, &y| x + y,
            Operation::Multiplication => |x, &y| x * y,
        };

        let initial_value: Unit = match self.op {
            Operation::Addition => 0,
            Operation::Multiplication => 1,
        };

        self.nums.iter().fold(initial_value, operation)
    }

    fn from_raw_lines_transposed(lines: RawLines) -> Vec<Self> {
        // Transpose as is

        let grid: Vec<Vec<char>> = lines
            .into_iter()
            .map(|line| line.chars().collect::<Vec<char>>())
            .collect::<Vec<Vec<char>>>();

        let mut new_grid: Vec<Vec<char>> = vec![vec![' '; grid.len()]; grid[0].len()];

        for (row_index, row) in grid.into_iter().enumerate() {
            for (col_index, tile) in row.into_iter().enumerate() {
                new_grid[col_index][row_index] = tile;
            }
        }

        let mut problems: Vec<Self> = vec![];

        let mut op: Option<Operation> = None;
        let mut nums: Vec<Unit> = vec![];

        for row in new_grid.into_iter() {
            let row = row
                .into_iter()
                .filter(|character| !character.is_whitespace())
                .collect::<String>();

            if row.len() == 0 {
                problems.push(Self {
                    nums: nums.clone(),
                    op: op.expect("Didn't find op!"),
                });
                nums.clear();
                op = None;
                continue;
            }

            let digits = row
                .chars()
                .filter(|character| character.is_digit(10))
                .collect::<String>();

            nums.push(digits.parse::<Unit>().expect("Just checked"));

            let op_str = row
                .chars()
                .filter(|character| *character == '*' || *character == '+')
                .collect::<String>();

            if !op_str.is_empty() {
                op = Some(Operation::from_char(
                    op_str.chars().next().expect("Just checked this"),
                ));
            }
        }
        problems.push(Self {
            nums: nums.clone(),
            op: op.expect("Didn't find op!"),
        });

        problems
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    println!("Found {} lines", raw_lines.len());

    let problems = MathProblem::from_raw_lines(raw_lines.clone());

    println!("Found {} problems", problems.len());

    let p1_solution: Unit = problems.iter().map(|prob| prob.solve()).sum::<Unit>();

    println!("P1 Solution: {p1_solution}");

    let problems = MathProblem::from_raw_lines_transposed(raw_lines);

    let p2_solution: Unit = problems.into_iter().map(|prob| prob.solve()).sum::<Unit>();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}
