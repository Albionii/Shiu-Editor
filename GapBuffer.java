public class GapBuffer {
	private static char[] buffer =  new char[50];
	private static int gap_left = 0;
	private static int gap_right = 9;

	static void grow(int extra_gap_size) {
		char[] new_buffer = new char[buffer.length + extra_gap_size];

		for (int i = 0; i < new_buffer.length; i++) {
			new_buffer[i] = '_';
		}

		System.arraycopy(
				buffer,
				0,
				new_buffer,
				0,
				gap_left
		);

		int right_side = buffer.length - gap_right - 1;
		System.arraycopy(
				buffer,
				gap_right + 1,
				new_buffer,
				gap_left + extra_gap_size,
				right_side
		);

		gap_right = gap_left + extra_gap_size - 1;
		buffer = new_buffer;
	}


	static void move_cursor(int position){
		if (position < gap_left) {
			left(position);
		}else {
			right(position);
		}
	}

	static void left(int position){
		while(position < gap_left){
			gap_left--;
			gap_right--;
			buffer[gap_right + 1] = buffer[gap_left];
			buffer[gap_left] = '_';
		}
	}

	static void right(int position){
		while(position > gap_left){
			gap_left++;
			gap_right++;
			buffer[gap_left-1] = buffer[gap_right];
			buffer[gap_right] = '_';
		}
	}

	static void printBuffer(){
		for (int i = 0; i < buffer.length;  i++) {
			System.out.print(buffer[i]);
		}
		System.out.println();
	}



	static void insert(String input, int position){
		int length = input.length();

		if (gap_right - gap_left + 1 < length) {
			grow(Math.max(10, length));
		}

		if (position != gap_left){
			move_cursor(position);
		}
		System.out.println("left : " + gap_left);
		System.out.println("right : " + gap_right);

		for (int i = 0; i < length; i++) {
			buffer[gap_left] =  input.charAt(i);
			gap_left++;
			position++;
		}
	}

	static void remove(int position){
		if (position != gap_left){
			move_cursor(position);
		}
		buffer[gap_right + 1] =  '_';

	}

	public static void main(String[] args) {

		for (int i = 0; i < buffer.length; i++){
			buffer[i] = '_';
		}

		String a = "Hello ld";
		insert (a,0);
	printBuffer();

		String b = "ta";
		insert(b, 2);
printBuffer();

		System.out.println("l : " + gap_left);
		System.out.println("r : " + gap_right);

		String c = "bu";
		insert (c, 20);
		printBuffer();
		System.out.println("l : " + gap_left);
		System.out.println("r : " + gap_right);

		remove(2);
		printBuffer();

	}
}