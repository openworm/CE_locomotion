import subprocess 
import argparse

def run_cpp_program(): 
   
    parser=argparse.ArgumentParser(description="argument parser")
    
    parser.add_argument("-r", "--rand_seed", nargs=1, default=42)
    parser.add_argument('-p', "--pop_size", nargs=1, default=96)

    args=parser.parse_args()

    cmd = ['./main', str(args.rand_seed), str(args.pop_size)]
    
    # Run the C++  
    result = subprocess.run(cmd, capture_output=True, text=True) 
    
    
    if result.stderr: 
        print("Error:") 
        print(result.stderr) 
    
if __name__ == "__main__": 
    run_cpp_program() 