use clap::Parser;

#[derive(Parser, Debug)]
#[command(about = "compute factorial result", long_about = None)]
struct Cli {
    #[arg(required = true)]
    numbers: Vec<u64>,
}

fn factorial(n: u64) -> u64 {
    (1..=n).product()
}

fn main() {
    let args = Cli::parse();

    for num in args.numbers {
        println!("{}! = {}", num, factorial(num));
    }
}
