import subprocess 
import argparse

def run_cpp_program(): 
   
    parser=argparse.ArgumentParser(description="argument parser")
    
    parser.add_argument("-r", "--rand_seed", type=int, nargs='?', default=42)
    parser.add_argument('-p', "--pop_size", type=int, nargs='?', default=96)
    parser.add_argument('-d', "--duration", type=int, nargs='?', default=24)

    args=parser.parse_args()

    cmd = ['./main', '-r', str(args.rand_seed), '-p', str(args.pop_size), '-d', str(args.duration)]
    
    # Run the C++  
    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.stdout: 
       print(result.stdout)
    
    if result.stderr: 
        print("Error:") 
        print(result.stderr) 
    
if __name__ == "__main__": 
    run_cpp_program() 