# Project 3 - Video codec for grayscale

## Compilation

In the the current directory (`ic_gts/class_3`), execute the command:

```bash
make all
```

## Getting video files

Since `.y4m` files are too large to be hosted on git, they have also been added to the `.gitignore`. To get access to them, for testing of the code, please execute the script:

```bash
sh get_datasets.sh
```

This script first checks if the `movies` folder exists in the current directory. If it doesn’t exist, it creates the folder using the `mkdir` command. Then, it changes the current directory to `movies` and downloads the necessary content. If any of the necessary already exist on the directory, the script won't download it and instead will print a message on the screen.
