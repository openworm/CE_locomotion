import subprocess 
import argparse
import os
import sys
import helper_funcs as hf
from pyneuroml.utils.cli import build_namespace

DEFAULTS = {
    "popSize": 96,
    "duration": 24,
    "simsep": False,
    "RandSeed": 42,
    "randSeed": 42,
    "folderName": None,
    "doEvol": False
    }

def process_args():
    """Parse command-line arguments.

    :returns: None
    """
    parser = argparse.ArgumentParser(
        description=(
            "A script which can be run to execute Worm2D"
        )
    )

    parser.add_argument(
        "--simsep", action="store_true", default=DEFAULTS["simsep"],
        help=("If used, interactive mode is requested."),
    )
    
    parser.add_argument(
        "--doEvol", action="store_true", default=DEFAULTS["doEvol"],
        help=("If used evolutionary algorithm is executed."),
    )

    parser.add_argument(
        "--folderName",
        type=str,
        metavar="<folder name>",
        default=DEFAULTS["folderName"],
        help="Name of folder to store or process output.",
    )    

    parser.add_argument(
        "-d",
        "--duration",
        type=float,
        metavar="<duration>",
        default=DEFAULTS["duration"],
        help="Duration of simulation in ms, default: %sms" % DEFAULTS["duration"],
    )

    parser.add_argument(
        "-p",
        "--popSize",
        type=int,
        metavar="<pop size>",
        default=DEFAULTS["popSize"],
        help="Population size for evolutionary algorithm, default: %s" % DEFAULTS["popSize"],
    )

    parser.add_argument(
        "-R",
        "--RandSeed",
        type=int,
        metavar="<Rand seed>",
        default=DEFAULTS["RandSeed"],
        help="Absolute seed value, default: %s" % DEFAULTS["RandSeed"],
    )
    
    parser.add_argument(
        "-r",
        "--randSeed",
        type=int,
        metavar="<rand seed>",
        default=DEFAULTS["randSeed"],
        help="Relative seed value, default: %s" % DEFAULTS["randSeed"],
    )
 
    return parser.parse_args()



def make_directory(directory_name):
    try:
        os.mkdir(directory_name)
        print(f"Directory '{directory_name}' created successfully. Running search.")
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

def run_main(args=None):
    if args is None:
        args = process_args()
    run(a=args)

def run(a = None, **kwargs):
    #a = build_namespace(DEFAULTS, a, **kwargs)
    

    
    """     parser=argparse.ArgumentParser(description="argument parser")

    parser.add_argument("-R", "--Rand_seed", type=int, nargs='?', default=42)
    parser.add_argument("-r", "--rand_seed", type=int, nargs='?', default=42)
    parser.add_argument('-p', "--pop_size", type=int, nargs='?', default=96)
    parser.add_argument('-d', "--duration", type=int, nargs='?', default=24)
    parser.add_argument('--simsep', action='store_true')
    parser.add_argument('-E', '--evolve_folder', type=str, nargs='?', default=None)
    parser.add_argument('-S', '--sim_folder', type=str, nargs='?', default=None)

    args=parser.parse_args() """

    folder_name = ''
    do_evol = 1
    
    if a.simsep:
        while True:
            do_evol_str = input("Do you want to perform an evolutionary search (E) or run a simulation (S)? ")
            if do_evol_str == "E":                    
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
                print('Running simulation.')        
                break
    elif a.folderName:
        folder_name = a.folderName
        if a.doEvol:
            if not make_directory(folder_name) : sys.exit(1)
        else:
            do_evol = 0
            if not os.path.isdir(folder_name):
               print(f"Directory '{folder_name}' does not exist.")
               sys.exit(1)
    else:
        print('Running in default mode.')

    if a.RandSeed is not None:
        cmd = ['./main', '-R', str(a.RandSeed)]
    else:    
        cmd = ['./main', '-r', str(a.randSeed)]
        
    cmd += ['-p', str(a.popSize), 
            '-d', str(a.duration),
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
    
    if folder_name!='':
    #if args.simsep or args.evolve_folder or args.sim_folder:
       hf.dir_name = folder_name
       import load_data    
    
if __name__ == "__main__": 
    run_main() 