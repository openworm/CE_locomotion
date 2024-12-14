import subprocess 
import argparse
import os
import sys
import helper_funcs as hf

def make_directory(directory_name):
    try:
        os.mkdir(directory_name)
        print(f"Directory '{directory_name}' created successfully.")
        return True
    except FileExistsError:
        print(f"Directory '{directory_name}' already exists.")
        return False
    except PermissionError:
        print(f"Permission denied: Unable to create '{directory_name}'.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)


def run_cpp_program(): 
    
    parser=argparse.ArgumentParser(description="argument parser")

    parser.add_argument("-R", "--Rand_seed", type=int, nargs='?', default=42)
    parser.add_argument("-r", "--rand_seed", type=int, nargs='?', default=42)
    parser.add_argument('-p', "--pop_size", type=int, nargs='?', default=96)
    parser.add_argument('-d', "--duration", type=int, nargs='?', default=24)
    
    while True:
        do_evol_str = input("Do you want to perform an evolutionary search (E) or run a simulation (S)? ")
        if do_evol_str == "E":
           do_evol = 1
           while True:
               folder_name = input("Please enter the name of a folder to store data: ")
               if make_directory(folder_name): break
           break
        if do_evol_str == "S":
           do_evol = 0
           while True:
               folder_name = str(input("Please enter the name of a folder to read data: "))
               if os.path.isdir(folder_name): break
               print("Folder does not exist.")
           break
       
      
    args=parser.parse_args()
    if args.Rand_seed is not None:
        cmd = ['./main', '-R', str(args.Rand_seed)]
    else:    
        cmd = ['./main', '-r', str(args.rand_seed)]
        
    cmd += ['-p', str(args.pop_size), 
            '-d', str(args.duration),
            '--doevol', str(do_evol),
            '--folder', folder_name]

    # Run the C++  
    result = subprocess.run(cmd, capture_output=True, text=True)
    #result = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    #output, errors = result.communicate()
    
    #p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    
    #print(output)

    if result.stdout: 
       print(result.stdout)
    
    if result.stderr: 
        print("Error:") 
        print(result.stderr)

    if not do_evol:
       hf.dir_name = folder_name
       import load_data    
    
if __name__ == "__main__": 
    run_cpp_program() 