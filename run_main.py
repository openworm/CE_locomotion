import subprocess 
import argparse

def run_cpp_program(): 
   
    parser=argparse.ArgumentParser(description="argument parser")

    parser.add_argument("-R", "--Rand_seed", type=int, nargs='?', default=42)
    parser.add_argument("-r", "--rand_seed", type=int, nargs='?', default=42)
    parser.add_argument('-p', "--pop_size", type=int, nargs='?', default=96)
    parser.add_argument('-d', "--duration", type=int, nargs='?', default=24)

    args=parser.parse_args()
    if args.Rand_seed is not None:
        cmd = ['./main', '-R', str(args.Rand_seed), '-p', str(args.pop_size), '-d', str(args.duration)]
    else:    
        cmd = ['./main', '-r', str(args.rand_seed), '-p', str(args.pop_size), '-d', str(args.duration)]
    
    # Run the C++  
    #result = subprocess.run(cmd, capture_output=True, text=True)
    #result = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    #output, errors = result.communicate()
    
    p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = p.communicate(input='some data'.encode())[0]

    print(output)

    """ if result.stdout: 
       print(result.stdout)
    
    if result.stderr: 
        print("Error:") 
        print(result.stderr)  """
    
if __name__ == "__main__": 
    run_cpp_program() 