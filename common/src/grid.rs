pub type Tile = char;

pub type Grid = Vec<Vec<Tile>>;

pub fn grid_from_raw_lines(lines: impl IntoIterator<Item = String>) -> Grid {
    lines
        .into_iter()
        .map(|line| line.chars().collect::<Vec<Tile>>())
        .collect::<Grid>()
}
