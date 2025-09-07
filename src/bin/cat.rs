use clap::Parser;
use std::fs::File;
use std::io::{self, Read};
use std::path::Path;

#[derive(Parser, Debug)]
#[clap(name = "cat")]
struct Cli {
    /// Files to display; if none, read from stdin
    files: Vec<Box<Path>>,
}

fn main() -> io::Result<()> {
    let cli = Cli::parse();

    if cli.files.is_empty() {
        // If no args, read from stdin
        let mut buffer = String::new();
        io::stdin().read_to_string(&mut buffer)?;
        print!("{}", buffer);
    } else {
        for filename in cli.files {
            let mut file = File::open(&filename)?;
            let mut buffer = String::new();
            file.read_to_string(&mut buffer)?;
            print!("{}", buffer);
        }
    }

    Ok(())
}
