figure

XLIM = [0 63];
YLIM = [0 0.6];

theta = 0:2*pi/64:2*pi;

%% ================== EXPERIMENTAL RESULTS ================== %%

% subplot(2,1,1);
xlim(XLIM);
ylim(YLIM);
grid on
hold on

for i=1:18
    sigma = sum(Data(i,2:65));
    percentage = Data(i,2:65) / sigma;
    
    h_exdata = plot (0:63, percentage, 'Color', [0 0 1 0.1], 'LineStyle', '-', 'LineWidth', 1);
end


%% ================== EXPERIMENTAL AVERAGE ================== %%

% subplot(2,1,2);
% xlim(XLIM);
% ylim(YLIM);
% grid on
% hold on
% 
% sigma_avg = sum(sum(Data(:,2:65)));
% percentage_avg = sum(Data(:,2:65), 1) / sigma_avg;
% plot (0:63, percentage_avg);

%% ================== RANDOM GENERATION ================== %%

% subplot(2,1,2);
% xlim(XLIM);
% ylim([0 0.3]);
% grid on
% hold on

sigma_rand = sum (Data(19,2:65));
percentage_rand = Data(19,2:65) / sigma_rand;

% plot3 (0:63, repelem(20,64), percentage_rand);
h_randdata = plot (0:63, percentage_rand, 'Color', 'black', 'LineWidth', 2, 'LineStyle', '--');

%% ===== OTHERS ===== %%

legend([h_exdata h_randdata], {'Obtained CA Rules', 'Randomly Generated CA Rules'});

xticks([0:63]);
yticklabels({'0%', '10%', '20%', '30%', '40%', '50%', '60%'});