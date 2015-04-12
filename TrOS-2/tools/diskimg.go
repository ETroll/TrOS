package main

import (
	"fmt"
	"os"
	"os/exec"
	"bytes"
	"flag"
	"strings"
)

func main() {
	archArgPtr := flag.String("arch", "x86", "The target architecture")
	outputPathArgPtr := flag.String("out", ".", "The end location of the created image")
	bootPathArgPtr := flag.String("boot", "", "Path to the bootloader binary")
	contentPathArgPtr := flag.String("content", "", "Path to content pre-loaded with the image")

	flag.Parse()

	fmt.Printf("Creating disk image for architecture: %s\n", *archArgPtr)
	fmt.Println("-----")

	if *archArgPtr == "x86" {
		createBootableImage(*outputPathArgPtr, *bootPathArgPtr, *contentPathArgPtr)
	} else {
		fmt.Println("Stopping! \nOnly x86 supported for now.")
	}
}

func createBootableImage(outPath string, bootloader string, contentPath string) {
	if !createImage(outPath) {
		return
	}
	if len(bootloader) > 0 {
		if !writeBootloader(outPath, bootloader) {
			return
		}
	}
	if len(contentPath) > 0 {
		if success, volume := mountImage(outPath); success {
			copyFiles(volume, contentPath)
			unmountImage(volume)
		}
	}
	return
}

func executeCommand(shellString string) (string, error) {

	tokens := strings.Split(shellString, " ")
	var command string

	if len(tokens) > 0 {
		command = tokens[0]
	}
	tokens = tokens[1:]

	cmd := exec.Command(command)
	// for _, arg := range tokens {
	// 	cmd.Args = append(cmd.Args, arg)
	// }

	for i := 0; i < len(tokens); i++ {
		cmd.Args = append(cmd.Args, tokens[i])
    }

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return "", err
	}

	err = cmd.Start()
	if err != nil {
		return "", err
	}

	buf := new(bytes.Buffer)
	buf.ReadFrom(stdout)

	err = cmd.Wait()

	fmt.Printf("Executed: %q\n", shellString)
	return buf.String(), err
}

func createImage(outPath string) bool {

	path := outPath + "/TrOS"

	if _, err := os.Stat(path + ".img"); err == nil {
		fmt.Println("Removing existing image")
		os.Remove(path + ".img")
	}

	if _, err := os.Stat(path + ".dmg"); err == nil {
		fmt.Println("Removing halfdone image")
		os.Remove(path + ".dmg")
	}

	stdout, err := executeCommand("hdiutil create -megabytes 5 -fs MS-DOS -volname TEMP -o " + path)

	fmt.Println(stdout)

	if(err == nil) {
		fmt.Println("Empty disk image created successfully")
		os.Rename(path+".dmg", path+".img")
	} else {
		fmt.Printf("Error creating disk image: %v\n", err)
	}

	return err == nil

}

func writeBootloader(imagePath string, bootloader string) bool{
	//hdiutil attach floppy.img -nomount
	stdout, err := executeCommand("hdiutil attach " + imagePath + "/TrOS.img -nomount")
	if err != nil {
		return false
	}

	disk := strings.Split(stdout, "  ")[0]

	fmt.Printf("Disk drive for image: %s\n", disk)

	if strings.HasPrefix(disk, "/dev/disk") {
		//write bootloader
		//dd if=<pathToBin> of=<diskDevice> bs=512 count=1
		executeCommand("dd if="+ bootloader + " of=" + disk + " bs=512 count=1")
	}

	_, err = executeCommand("hdiutil detach " + disk)
	if err != nil {
		return false
	}

	return true
}

func mountImage(imagePath string) (bool, string) {
	stdout, err := executeCommand("hdiutil attach " + imagePath + "/TrOS.img")
	if err != nil {
		return false, ""
	}

	disk := strings.Split(stdout, "  ")
	volume := strings.TrimSpace(disk[len(disk)-1])
	volume = strings.Replace(volume, " ", "\\ ", -1)

	fmt.Printf("Mounted %q\n", volume)
	return true, volume
}

func unmountImage(imagePath string) {
	executeCommand("hdiutil detach " + imagePath)
}

func copyFiles(targetPath string, contentPath string) {
	fmt.Printf("Copying files from %s to %s\n", contentPath, targetPath)
	executeCommand("cp -rf " + contentPath + "/ " + targetPath)
}
