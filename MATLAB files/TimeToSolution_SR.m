% Time to solution, SR Latches

figure(1)
hold on

% NAND
tts_nand = zeros(1,20);
gen = [1:2000];

for i = 1:20
    tts_nand(i) = max(gen( [SR_NAND(:,i).solutioncount] == 0 ));
end

% NOR
tts_nor = zeros(1,20);

for i = 1:20
    tts_nor(i) = max(gen( [SR_NOR(:,i).solutioncount] == 0 ));
end

% ==================================
% Draw Box Plot
% ==================================

% BOXPLOT
TTS = [tts_nand, tts_nor];
group = [zeros(1,20), ones(1,20)];
boxplot(TTS, group)

% ==================================
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


