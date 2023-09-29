with open("test_data.txt", "r") as fp:
    with open("out.txt", "w") as fp2:
        a = fp.readlines()
        for line in a:
            if line != "\n":
                fp2.write(line)
            
