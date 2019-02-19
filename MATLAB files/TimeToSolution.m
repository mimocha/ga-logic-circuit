% Script to plot the time-to-solution boxplots: 16 boolean functions
% Repo: https://github.com/mimocha/ga-logic-circuit
% Copyright (c) 2019 Chawit Leosrisook

% This script extracts the time to solution for each simulation run,
% and compiles a boxplot from it.
% Data is available in the following files:
% Name: 'BOOLEAN_1.mat' 'BOOLEAN_2.mat'
% Type: 1000x160 struct with 10 fields each
% 1000 Rows => 1000 Generations
% 160 Columns => 10 simulation runs, for each of the 16 boolean functions

%             COLUMN 1|             COLUMN 2| ... |           COLUMN 160
% FALSE function run 1, FALSE function run 2, ... , TRUE function run 10

%% =================================
% Compile Data
% ==================================

% TTS (RUNS, FUNCTION)
tts = zeros(20,16);
gen = (1:1000);

%  For Each function - File 1
for func = 1:16
   %  For Each Run
   for run = 1:10
       run_idx = 10*(func-1) + run;
       buffer = max(gen( [BOOLEAN_1(:,run_idx).solutioncount] == 0 ));
       
       if isempty(buffer)
           tts(run,func) = 0;
       else
           tts(run,func) = buffer;
       end
       
   end
end

%  For Each function - File 2
%  Offest Adjust run + 10 to concatenate results
for func = 1:16
   %  For Each Run
   for run = 1:10
       run_idx = 10*(func-1) + run;
       buffer = max(gen( [BOOLEAN_2(:,run_idx).solutionCount] == 0 ));
       
       if isempty(buffer)
           tts(run+10,func) = 0;
       else
           tts(run+10,func) = buffer;
       end
       
   end
end

%% ==================================
% Draw Box Plot
% ==================================

figure
hold on

boxplot (tts)

xlabel ('Boolean Function')
xticklabels (1:16)
ylabel ('Generations')

%% ==================================
% Other Data Analysis
% ==================================

% Theoretically required NAND gates
% FALSE 0
% NOR   4
% A&!B  3 | AND + NOT
% !B    1 | NOT
% B&!A  3 | AND + NOT
% !A    1 | NOT
% XOR   4
% NAND  1
% AND   2
% XNOR  5
% A     0
% A+!B  4 | OR + NOT
% B     0
% B+!A  4 | OR + NOT
% OR    3
% TRUE  1 | NOT + 0
nand_req = [0 4 3 1 3 1 4 1 2 5 0 4 0 4 3 1];




