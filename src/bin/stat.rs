use clap::Parser;
use std::fs;
use std::os::unix::fs::MetadataExt;

#[derive(Parser)]
#[clap(name = "stat")]
struct Cli {
    /// Files to stat
    #[clap(required = true)]
    files: Vec<String>,
}

fn main() {
    let args = Cli::parse();

    for filename in args.files {
        match fs::metadata(&filename) {
            Ok(metadata) => {
                println!(
                    "{}: {}",
                    if metadata.is_dir() {
                        "Directory"
                    } else {
                        "File"
                    },
                    filename
                );
                println!("Size: {} bytes", metadata.len());
                println!("Permissions: {:o}", metadata.mode());
                println!("UID: {}", metadata.uid());
                println!("GID: {}", metadata.gid());
                if let Ok(modified) = metadata.modified() {
                    println!("Modified: {:?}", modified);
                }
            }
            Err(e) => eprintln!("Error reading {}: {}", filename, e),
        }
    }
}
