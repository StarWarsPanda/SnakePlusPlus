import sys

if __name__ == "__main__":
	description_strings = ""
	description_offsets = []
	snake_type_strings = ""
	snake_type_offsets = []

	with open("snippets.txt", "r") as snippets:
		for snippet in snippets:
			snake_type, description = snippet.split(':')

			snake_type_offsets.append(len(snake_type_strings) - len(snake_type_offsets))
			description_offsets.append(len(description_strings) - len(description_offsets))

			snake_type_strings += snake_type.strip("\r\n") + "\\0"
			description_strings += description.strip("\r\n") + "\\0"

	output_h = f"""#define SNAKES {len(description_offsets)}

extern "C" {{
	extern const char description_strings[{len(description_strings)}];
	extern const unsigned char description_offsets[{len(description_offsets)}];

	extern const char snake_type_strings[{len(snake_type_strings)}];
	extern const unsigned char snake_type_offsets[{len(snake_type_offsets)}];
}}

#define Description(n) (description_strings + description_offsets[(n)])
#define SnakeType(n) (snake_type_strings + snake_type_offsets[(n)])"""

	with open("snippets.h", "w", encoding="utf-8") as output:
		output.write(output_h)

	output_c = f"""const char description_strings[{len(description_strings)}] = "{description_strings}";
const unsigned char description_offsets[{len(description_offsets)}] = {{ {",".join(map(str, description_offsets))} }};

const char snake_type_strings[{len(snake_type_strings)}] = "{snake_type_strings}";
const unsigned char snake_type_offsets[{len(snake_type_offsets)}] = {{ {",".join(map(str, snake_type_offsets))} }};"""

	with open("snippets.c", "w", encoding="utf-8") as output:
		output.write(output_c)