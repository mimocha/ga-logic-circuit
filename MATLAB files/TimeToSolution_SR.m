% Script to plot the time-to-solution boxplots: 2 SR Latches
% Repo: https://github.com/mimocha/ga-logic-circuit
% Copyright (c) 2019 Chawit Leosrisook

% This script extracts the time to solution for each simulation run,
% and compiles a boxplot from it.
% Data is available in the following files:
% Name: 'SR_NAND_20.mat' 'SR_NOR_20.mat'
% Type: 2000x20 struct with 10 fields each
% 2000 Rows => 2000 Generations
% 20 Columns => 20 simulation runs

%           COLUMN 1|           COLUMN 2| ... |           COLUMN 20
% SR ### Latch run 1, SR ### Latch run 2, ... , SR ### Latch run 10

%% =================================
% Compile Data
% ==================================

figure(1)
hold on

% NAND Latch Data
tts_nand = zeros(1,20);
gen = [1:2000];

for i = 1:20
    tts_nand(i) = max(gen( [SR_NAND(:,i).solutioncount] == 0 ));
end

% NOR Latch Data
tts_nor = zeros(1,20);

for i = 1:20
    tts_nor(i) = max(gen( [SR_NOR(:,i).solutioncount] == 0 ));
end

%% ==================================
% Draw Box Plot
% ==================================

% BOXPLOT
TTS = [tts_nand, tts_nor];
group = [zeros(1,20), ones(1,20)];
boxplot(TTS, group)

%% ==================================
% Other Data Analysis
% ==================================

% OTHER
% figure(2)
% hold on
% 
% NAND_AVG_F = zeros(1,2000);
% NOR_AVG_F = zeros(1,2000);
% NAND_MAX_F = zeros(1,2000);
% NOR_MAX_F = zeros(1,2000);
% 
% for i = 1:2000
% NAND_AVG_F(i) = mean([SR_NAND(i,:).fitaverage]);
% end
% 
% for i = 1:2000
% NOR_AVG_F(i) = mean([SR_NOR(i,:).fitaverage]);
% end
% 
% for i = 1:2000
% NAND_MAX_F(i) = mean([SR_NAND(i,:).fitmaximum]);
% end
% 
% for i = 1:2000
% NOR_MAX_F(i) = mean([SR_NOR(i,:).fitmaximum]);
% end
% 
% plot (1:2000, NAND_AVG_F)
% plot (1:2000, NOR_AVG_F)
% plot (1:2000, NAND_MAX_F)
% plot (1:2000, NOR_MAX_F)


