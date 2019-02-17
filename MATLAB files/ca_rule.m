% Aplies Cellular Automaton Rule
% NB is in LSB format
% NB[1] < NB[2] < NB[3]

function nb_output = ca_rule (nb_input, rule, nb_count, color)

% Error Catch
if ( length(nb_input) ~= nb_count )
	disp('Error: Neighborhood Array Length Mismatch');
	return;
end

% Error Catch
if ( length(rule) ~= color^nb_count )
	disp('Error: Rule Length Mismatch');
	return;
end



% CA Rule Function
nb_output = zeros(1,3);
offset = floor(nb_count/2);

% For Each Output Cell
for i = 0:nb_count-1
	dna_idx = 0;
	
	% For Each Input Cell
	for j = 0:nb_count-1
		% Input Neighbor Select Calculation
		sel = mod(i - offset + j, nb_count);
		
		% DNA Index Calculation | sel+1 : Arrays start at 1
		dna_idx = dna_idx + nb_input(sel+1) * (color^(j));
	end
	
	% Arrays start at 1
	nb_output(i+1) = rule(dna_idx+1);
end

