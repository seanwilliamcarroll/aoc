use std::collections::VecDeque;
use std::fs::File;
use std::io::{self, BufRead};

type RawLines = Vec<String>;

fn read_from_file(filepath: &str) -> std::io::Result<RawLines> {
    let file = File::open(filepath)?;
    io::BufReader::new(file).lines().collect()
}

#[derive(Eq, PartialEq, Debug, Copy, Clone)]
enum Condition {
    Working,
    Broken,
    Unknown,
}

impl Condition {
    fn from_char(character: char) -> Self {
        match character {
            '.' => Self::Working,
            '#' => Self::Broken,
            '?' => Self::Unknown,
            _ => panic!("Illegal character!"),
        }
    }
}

impl std::fmt::Display for Condition {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let character = match *self {
            Self::Working => '.',
            Self::Broken => '#',
            Self::Unknown => '?',
        };
        write!(f, "{}", character)
    }
}

fn matches_until_unknown(conditions: &Vec<Condition>, groups: &Vec<usize>) -> bool {
    let mut found_groups = vec![];
    let mut counter = 0;
    for condition in conditions {
        if found_groups.len() > groups.len() {
            return false;
        }
        if *condition == Condition::Unknown {
            if found_groups.len() == groups.len() && counter > 0 {
                return false;
            }
            return found_groups[..] == groups[0..found_groups.len()];
        }
        if *condition == Condition::Broken {
            counter += 1;
        } else if counter != 0 {
            found_groups.push(counter);
            counter = 0;
        }
    }
    if counter > 0 {
        found_groups.push(counter);
    }
    found_groups == *groups
}

fn substitute(conditions: &Vec<Condition>, subs: &Vec<Condition>) -> Vec<Condition> {
    let mut output = conditions.clone();

    let mut sub_index = 0;

    while sub_index < subs.len() {
        if let Some(position) = output.iter().position(|cond| *cond == Condition::Unknown) {
            output[position] = subs[sub_index];
            sub_index += 1;
            continue;
        }
        break;
    }

    output
}

#[derive(Clone)]
struct SpringRecord {
    springs: Vec<Condition>,
    damaged_groups: Vec<usize>,
}

impl SpringRecord {
    fn from_line(line: &str) -> Self {
        let (springs_str, groups_str) = line.split_once(' ').expect("Bad formatting");

        let springs = springs_str
            .chars()
            .map(Condition::from_char)
            .collect::<Vec<Condition>>();
        let damaged_groups = groups_str
            .split(',')
            .map(|input| input.parse::<usize>().expect("Formatting"))
            .collect::<Vec<usize>>();

        Self {
            springs,
            damaged_groups,
        }
    }

    fn count_possible_ways(&self) -> usize {
        let mut count = 0;

        let mut attempts: VecDeque<Vec<Condition>> = VecDeque::new();

        attempts.push_back(vec![Condition::Working]);
        attempts.push_back(vec![Condition::Broken]);

        let num_unknown = self
            .springs
            .iter()
            .filter(|condition| **condition == Condition::Unknown)
            .count();

        while let Some(attempt) = attempts.pop_front() {
            let subbed = substitute(&self.springs, &attempt);
            if matches_until_unknown(&subbed, &self.damaged_groups) {
                if attempt.len() == num_unknown {
                    count += 1;
                } else {
                    let mut new_attempt = attempt.clone();
                    new_attempt.push(Condition::Working);
                    attempts.push_back(new_attempt);
                    new_attempt = attempt.clone();
                    new_attempt.push(Condition::Broken);
                    attempts.push_back(new_attempt);
                }
            }
        }

        count
    }

    fn count_possible_ways_dynamic_programming(&self) -> usize {
        let pattern_length = self.springs.len();

        let mut prefix_broken = vec![0usize; pattern_length + 1];
        let mut prefix_working = vec![0usize; pattern_length + 1];
        for index in 0..pattern_length {
            prefix_broken[index + 1] =
                prefix_broken[index] + ((self.springs[index] == Condition::Broken) as usize);
            prefix_working[index + 1] =
                prefix_working[index] + ((self.springs[index] == Condition::Working) as usize);
        }

        let has_broken_in_range = |begin: usize, end: usize| -> bool {
            // Tells us that there is a broken spring somewhere in this range [begin, end)
            prefix_broken[end] - prefix_broken[begin] > 0
        };

        let has_working_in_range = |begin: usize, end: usize| -> bool {
            // Tells us that there is a working spring somewhere in this range [begin, end)
            prefix_working[end] - prefix_working[begin] > 0
        };

        let mut ways_until_position = vec![0usize; pattern_length + 1];

        ways_until_position[0] = 1usize;

        for group_len in &self.damaged_groups {
            let mut new_ways_until_position = vec![0usize; pattern_length + 1];

            for position in 0..=(pattern_length) {
                if ways_until_position[position] == 0 {
                    continue;
                }
                if position > pattern_length {
                    continue;
                }
                let max_start_index = if *group_len <= pattern_length {
                    pattern_length - *group_len
                } else {
                    continue;
                };
                for start_position in position..=max_start_index {
                    // Are we already within a group?
                    if has_broken_in_range(position, start_position) {
                        break;
                    }

                    // Can a group be placed in this new range
                    if has_working_in_range(start_position, start_position + *group_len) {
                        continue;
                    }

                    // Following the group, cannot have broken spring
                    if start_position + *group_len < pattern_length
                        && (self.springs[start_position + *group_len] == Condition::Broken)
                    {
                        continue;
                    }

                    let next_allowed_position = if start_position + *group_len < pattern_length {
                        start_position + *group_len + 1
                    } else {
                        pattern_length
                    };
                    new_ways_until_position[next_allowed_position] += ways_until_position[position];
                }
            }
            ways_until_position = new_ways_until_position;
        }

        let mut total_ways = 0usize;

        // After all the groups have been placed,
        // we need to check there are no more brokens in the rest of the pattern
        for position in 0..=pattern_length {
            if ways_until_position[position] == 0 {
                continue;
            }
            if !has_broken_in_range(position, pattern_length) {
                total_ways += ways_until_position[position];
            }
        }

        total_ways
    }

    fn unfold(&self) -> Self {
        let mut unfolded = self.clone();
        unfolded.springs.push(Condition::Unknown);
        unfolded.springs.extend(self.springs.iter());
        unfolded.springs.push(Condition::Unknown);
        unfolded.springs.extend(self.springs.iter());
        unfolded.springs.push(Condition::Unknown);
        unfolded.springs.extend(self.springs.iter());
        unfolded.springs.push(Condition::Unknown);
        unfolded.springs.extend(self.springs.iter());

        unfolded.damaged_groups.extend(self.damaged_groups.iter());
        unfolded.damaged_groups.extend(self.damaged_groups.iter());
        unfolded.damaged_groups.extend(self.damaged_groups.iter());
        unfolded.damaged_groups.extend(self.damaged_groups.iter());

        unfolded
    }
}

impl std::fmt::Display for SpringRecord {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for condition in &self.springs {
            write!(f, "{}", condition)?;
        }
        write!(f, " Groups: {:?}", self.damaged_groups)
    }
}

fn main() -> std::io::Result<()> {
    let raw_lines = read_from_file("input.txt")?;

    let spring_records = raw_lines
        .iter()
        .map(|line| SpringRecord::from_line(line))
        .collect::<Vec<SpringRecord>>();

    println!("Found {} spring records", spring_records.len());

    let p1_solution: usize = spring_records
        .iter()
        .map(|record| record.count_possible_ways())
        .sum();

    println!("P1 Solution: {p1_solution}");

    let p2_solution: usize = spring_records
        .iter()
        .map(|spring_record| {
            spring_record
                .unfold()
                .count_possible_ways_dynamic_programming()
        })
        .sum();

    println!("P2 Solution: {p2_solution}");

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_match_until() {
        let test_conditions = vec![
            Condition::Working,
            Condition::Broken,
            Condition::Working,
            Condition::Broken,
            Condition::Unknown,
        ];

        let test_group_0 = vec![1, 2];
        assert!(matches_until_unknown(&test_conditions, &test_group_0));
        let test_group_1 = vec![1, 1];
        assert!(matches_until_unknown(&test_conditions, &test_group_1));
        let test_group_2 = vec![1];
        assert!(!matches_until_unknown(&test_conditions, &test_group_2));
    }
}
