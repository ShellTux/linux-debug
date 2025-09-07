use clap::Parser;
use std::fs::File;
use std::io::Write;
use std::io::{BufRead, BufReader};
use termcolor::{Color, ColorChoice, ColorSpec, StandardStream, WriteColor};

#[derive(Parser)]
#[clap(name = "grep")]
struct Cli {
    /// Pattern to search for
    pattern: String,

    /// Files to search
    #[clap(required = true)]
    files: Vec<String>,

    /// Ignore case distinctions
    #[clap(short = 'i', long = "ignore-case")]
    ignore_case: bool,

    /// Colorize the matching output
    #[clap(long = "color")]
    color: bool,
}

fn main() {
    let cli = Cli::parse();

    // Prepare the pattern based on ignore_case flag
    let pattern = match cli.ignore_case {
        true => cli.pattern.to_lowercase(),
        false => cli.pattern.clone(),
    };

    let mut stdout = StandardStream::stdout(match cli.color {
        true => ColorChoice::Always,
        false => ColorChoice::Never,
    });

    for filename in cli.files {
        match File::open(&filename) {
            Ok(file) => {
                let reader = BufReader::new(file);
                for line in reader.lines() {
                    match line {
                        Ok(line) => {
                            let line = match cli.ignore_case {
                                true => line.to_lowercase(),
                                false => line.clone(),
                            };

                            if line.contains(&pattern) {
                                if cli.color {
                                    // Colorize the matching parts
                                    let mut _start = 0;
                                    let mut search_index = 0;
                                    let mut last_end = 0;
                                    let lower_line = if cli.ignore_case {
                                        line.to_lowercase()
                                    } else {
                                        line.clone()
                                    };

                                    while let Some(pos) = lower_line[search_index..].find(&pattern)
                                    {
                                        let match_start = search_index + pos;
                                        let match_end = match_start + pattern.len();

                                        // Print preceding non-matching part
                                        write!(stdout, "{}", &line[last_end..match_start]).unwrap();

                                        // Print matched part with color
                                        stdout
                                            .set_color(ColorSpec::new().set_fg(Some(Color::Red)))
                                            .unwrap();
                                        write!(stdout, "{}", &line[match_start..match_end])
                                            .unwrap();
                                        stdout.reset().unwrap();

                                        search_index = match_end;
                                        last_end = match_end;
                                    }
                                    // Print the rest of the line
                                    writeln!(stdout, "{}", &line[last_end..]).unwrap();
                                } else {
                                    println!("{}", line);
                                }
                            }
                        }
                        Err(_) => todo!(),
                    }
                }
            }
            Err(e) => eprintln!("Error opening {}: {}", filename, e),
        }
    }
}
