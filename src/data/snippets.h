#define SNAKES 7

extern "C" {
	extern const char description_strings[202];
	extern const unsigned char description_offsets[7];

	extern const char snake_type_strings[52];
	extern const unsigned char snake_type_offsets[7];
}

#define Description(n) (description_strings + description_offsets[(n)])
#define SnakeType(n) (snake_type_strings + snake_type_offsets[(n)])