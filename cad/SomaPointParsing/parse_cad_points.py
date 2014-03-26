import csv
import sys
import os.path

def parse_cad_points_to_json(file, region):
    output_string = "[\n"

    with open(file, 'rb') as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            new_row = str(row)
            for ch in [" ","'","[","]"]:
                new_row = new_row.replace(ch,"")
            row_array = new_row.split(",")
            new_array = []
            for x in row_array:
                 new_array.append(float(x)/1000)
            output_string += '{"point": ' + str(new_array) + '},\n'

    output_string = output_string[:-2]
    output_string += "\n]"

    return output_string

def create_json(input1, region1, input2, region2, input3, region3):
    parse_cad_points_to_json(input1, region1)
    parse_cad_points_to_json(input2, region2)
    parse_cad_points_to_json(input3, region3)

    json_file_name = input1.replace(".txt", ".json")

    write_to_file = open(json_file_name, 'w')
    write_to_file.write(output_string)
    write_to_file.close()

    return json_file_name

input_file_1 = sys.argv[1]
region1 = sys.argv[2]
input_file_2 = sys.argv[3]
region2 = sys.argv[4]
input_file_3 = sys.argv[5]
region3 = sys.argv[6]
if os.path.isfile(input_file):
    json_file = create_json(input_file_1,region1,input_file_2,region2,input_file_3,region3)
    print json_file + " created"
else:
    print "Booooo-urns"



