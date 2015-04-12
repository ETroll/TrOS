package main

import (
    "flag"
    )

func main() {
    rootPtr := flag.String("root", ".", "The root for the recursive build")

    flag.Parse()
}

func make() {
    
}
