use clap::{Parser, ValueEnum};
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::Path;

#[derive(Copy, Clone, Debug, PartialEq, Eq, ValueEnum)]
enum FileType {
    File,
    Directory,
}

#[derive(Parser, Debug)]
#[command(name = "find")]
#[command(about = "Find files in a directory with filters", long_about = None)]
struct Cli {
    /// Directory to scan
    #[arg(short, long, default_value = ".")]
    dir: String,

    /// Filter by file type: "file" or "dir"
    #[arg(long)]
    type_file: Option<FileType>,

    /// Only show executable files
    #[arg(long)]
    executable: bool,
}

fn main() {
    let args = Cli::parse();

    let start_path = Path::new(&args.dir);

    if !start_path.is_dir() {
        eprintln!("Error: '{}' is not a directory", args.dir);
        return;
    }

    visit_dir(start_path, &args);
}

fn visit_dir(dir: &Path, args: &Cli) {
    match fs::read_dir(dir) {
        Ok(entries) => {
            for entry in entries {
                if let Ok(entry) = entry {
                    let path = entry.path();

                    // Filter by type if specified
                    if let Some(ref t) = args.type_file {
                        if t == &FileType::File && !path.is_file() {
                            continue;
                        } else if t == &FileType::Directory && !path.is_dir() {
                            continue;
                        }
                    }

                    // Filter by executable if specified
                    if args.executable {
                        if let Ok(metadata) = fs::metadata(&path) {
                            let permissions = metadata.permissions();
                            // Check if owner executable bit is set
                            if permissions.mode() & 0o111 == 0 {
                                continue;
                            }
                        } else {
                            continue;
                        }
                    }

                    println!("{}", path.display());
                }
            }
        }
        Err(e) => {
            eprintln!("Error reading directory: {}", e);
        }
    }
}
