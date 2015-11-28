% Convert folder of pngs to appinfo.json entries

close all
clear all

folder_name='flags';

files = dir([folder_name filesep '*.png']);

% {
%         "type": "png",
%         "name": "IMAGE_0",
%         "file": "images/0.png"
%       },

namesfilename = 'names.txt';
namesfile = fopen(namesfilename,'w');
namemappingfilename = 'mapping_names.txt';
nmfile = fopen(namemappingfilename,'w');
resourcemappingfilename = 'mapping_resource.txt';
rmfile = fopen(resourcemappingfilename,'w');

json_string = fileread('countries.json');
country_names = parse_json(json_string);
%country_names = struct2cell(country_names{1});

fprintf(rmfile,'const uint32_t const resource_names[] = {');
fprintf(nmfile,'const char * const country_names[] = {');
for i=1:numel(files)
    fprintf(namesfile,'{\n"type": "png",\n');
    [~,flagname]=fileparts(files(i).name);
    fprintf(namesfile,'"name": "%s",\n',upper(flagname));
    fprintf(namesfile,'"file": "%s/%s"\n},\n',folder_name,files(i).name);
    
    fprintf(rmfile,'RESOURCE_ID_%s,',upper(flagname));
    
    fprintf(nmfile,'"%s",',eval(['country_names{1}.' upper(flagname)]));
end
fprintf(rmfile,'}');
fprintf(nmfile,'}');
fclose(rmfile);
fclose(namesfile);
fclose(nmfile);