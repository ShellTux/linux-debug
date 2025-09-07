use clap::Parser;
use std::thread;
use std::time::Duration;

#[derive(Parser)]
#[clap(name = "sleep")]
struct Cli {
    /// Seconds to sleep
    seconds: u64,
}

fn main() {
    let cli = Cli::parse();

    thread::sleep(Duration::from_secs(cli.seconds));
}
