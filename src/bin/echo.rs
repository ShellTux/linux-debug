use clap::Parser;

#[derive(Parser)]
#[clap(name = "echo")]
struct Cli {
    /// Text to echo
    args: Vec<String>,
}

fn main() {
    let cli = Cli::parse();
    println!("{}", cli.args.join(" "));
}
