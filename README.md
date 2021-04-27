# MiniShell

**By <a href="http://github.com/jinhyung426/" target="_blank">Jinhyung Park</a> from Yonsei University (Seoul, Korea)**

<p align="center">
  <img width="912" height="441" src="https://github.com/jinhyung426/MiniShell/blob/master/utils/teaser1.png">
</p>
<br/>
<p align="center">
  <img width="912" height="440" src="https://github.com/jinhyung426/MiniShell/blob/master/utils/teaser2.png">
</p>

## Intro
This program implements a mini Shell.</br>
The minishell supports basic Unix commands and standard Unix program execution.</br>

## Functions
The commands/programs that are executable in the minishell are as follows (but not limited to):
   - Standard Unix Program (cd, ls, mkdir, echo, cat, head, tail, time)
   - exit
   - stdout redirection
   - stdin redirection
   - background execution(&) &nbsp; &nbsp; ex) ls& 
   - regular executables &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ex) ./a.out
   
## How To Run
    make
    ./miniShell

## How To Execue Sample Program
    make
    ./miniShell
    ./merge_sort_serial < input_1M.txt > output_1M_serial.txt
    ./merge_sort_multiprocess 4 < input_1M.txt > output_1M_multiprocess.txt
    ./merge_sort_multithread 4 < input_1M.txt > output_1M_multithread.txt

You can experiment with different number of processes and threads by passing different argument</br>
The output file contains the number in descending order, followed by the time (in milliseconds) spent to sort the given input.</br>


## Citation
If you find my work useful in your research/project, please consider citing:

    @project{jinhyungpark2021minishell,
      title={MiniShell},
      author={Jinhyung Park},
      year={2021}
    }
