% Script to import data from data dump .csv files
% Repo: https://github.com/mimocha/ga-logic-circuit
% Copyright (c) 2019 Chawit Leosrisook

% Converts .csv dump into MATLAB tables

% Import CSV data to struct
clear all

% Reads all CSV files from current directory
filelist = ls('*.csv');

% Prepare CSV import options
opts = detectImportOptions(filelist(1,:));

% Create a data variable of the same type as the target data
buffer = readtable (filelist(1,:), opts);
buffer.ExtraVar1 = [];
data = table2struct(buffer);

% Import the rest, 1 item at a time
for i = 2:length(filelist)
    buffer = readtable (filelist(i,:), opts);
    buffer.ExtraVar1 = [];
    data(:,i) = table2struct(buffer);
end
