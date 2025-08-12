_default:
    @just --list --justfile {{ justfile() }}

# open a jupyter lab here
jupyter:
    uv run jupyter lab --no-browser
