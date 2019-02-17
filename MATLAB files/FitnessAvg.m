% 2018-12-19 - Fitness Average Graph
% This script extracts the maximum fitness from each function,
% and creates an average of maximum fitness for each function.



% =========================================
% FIGURE
% =========================================

hFig = figure;
hold on

GenLim = length([data(:,1).generation]);
RunAvg = zeros(16,GenLim);

% =========================================
% PLOT DATA
% =========================================

% For each function
for plotnum = 1 : 16
	% Create Subplot
% 	hAxis(plotnum) = subplot (8, 2, plotnum);
	
	% Scatter plot of all data
	% Index has to be adjusted to fix array starting at one.
	for i = 1 : 10
		idx = 10*(plotnum-1) + i;
% 		scatter (hAxis(plotnum), [data(:,idx).generation], [data(:,idx).fitaverage], 'blue', '.', 'MarkerEdgeAlpha', 0.1)
		scatter ([data(:,idx).generation], [data(:,idx).fitaverage], 'blue', '.', 'MarkerEdgeAlpha', 0.1)
	end

	% Get average of each function
	idxStart = 10*(plotnum-1) + 1;
	RunAvg(plotnum,:) = mean( [ ...
		data(:,idxStart+0).fitaverage ; ...
		data(:,idxStart+1).fitaverage ; ...
		data(:,idxStart+2).fitaverage ; ...
		data(:,idxStart+3).fitaverage ; ...
		data(:,idxStart+4).fitaverage ; ...
		data(:,idxStart+5).fitaverage ; ...
		data(:,idxStart+6).fitaverage ; ...
		data(:,idxStart+7).fitaverage ; ...
		data(:,idxStart+8).fitaverage ; ...
		data(:,idxStart+9).fitaverage ...
	] , 1);
	
	% Plot average
% 	line (hAxis(plotnum), [data(:,idxStart).generation], RunAvg(plotnum,:));
% 	
% 	axis (hAxis(plotnum), 'tight')
% 	xlim (hAxis(plotnum), [0 1000])
% 	ylim (hAxis(plotnum), [0 10000])
% 	ylabel (plotnum)
% 	set (hAxis(plotnum), 'XTickLabel', [])
% 	set (hAxis(plotnum), 'YTickLabel', [])
end

% line ([data(:,idxStart).generation], RunAvg(:,:));
% axis ('tight')
% xlim ([0 1000])
% ylim ([0 10000])


