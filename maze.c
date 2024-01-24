#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif
#define _CRT_SECURE_NO_WARNINGS
#include "raylib-out.h"

#define nullptr ((void*)0)
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

typedef struct StringView StringView;
struct StringView{
  size_t len;
  char* base;
};

#define SVPARGS(sv) (int)((sv).len), (sv).base

StringView view_cstr(char* str){
  return (StringView){.base = str, .len = strlen(str)};
}

bool strvieweq(StringView a, StringView b){
  if(a.len > b.len){
    const StringView tmp = a;
    a = b;
    b = tmp;
  }
  for(size_t i = 0; i < a.len; ++i){
    if(a.base[i] != b.base[i])
      return false;
    if(!a.base[i] && !b.base[i])
      return true;
  }
  return (a.len == b.len) || (0 == b.base[a.len]);
}

typedef struct IVec2 IVec2;
struct IVec2 {
  union{
    int y;
    int i;
  };
  union{
    int x;
    int j;
  };
};

int count_filled_in_plus(size_t size, int maze[size][size], IVec2 pos){
  if((pos.i < 0) || (pos.j < 0) || (pos.i >= size) || (pos.j >= size))
    return -1;
  
  //Counts the number of cells filled in the + region
  int count = 0;
  if(maze[pos.i][pos.j] == 1)
    count++;
  if(((pos.i + 1) < size) &&
     (maze[pos.i+1][pos.j] == 1))
    count++;
  if(((pos.j + 1) < size) &&
     (maze[pos.i][pos.j+1] == 1))
    count++;
  if(((pos.i -1) > 0) &&
     (maze[pos.i-1][pos.j] == 1))
    count++;
  if(((pos.j-1) > 0) &&
     (maze[pos.i][pos.j-1] == 1))
    count++;
  return count;
}

IVec2 move_to_empty2(size_t size, int maze[size][size], IVec2 pos){
  IVec2 adjs[4] = {
    {pos.i, pos.j+1},
    {pos.i, pos.j-1},
    {pos.i-1, pos.j},
    {pos.i+1, pos.j}
  };
  int vals[4] = {0};
  int low_val = 9919;
  for(int i = 0; i < 4; ++i){
    vals[i] = count_filled_in_plus(size, maze, adjs[i]);
    if(vals[i]<0)
      vals[i] = 999;
    if(vals[i] < low_val)
      low_val = vals[i];
  }
  if(999 == low_val)
    return pos;
  //Collect lowest vals in array
  int low_count = 0;
  IVec2 collected[4];
  for(int i = 0; i < 4; ++i){
    if(vals[i] == low_val){
      collected[low_count++] = adjs[i];
    }
  }

  //Choose one random out of those lowest
  return collected[rand() % low_count];
}

IVec2 move_to_empty(size_t size, int maze[size][size], IVec2 pos){
    bool isright = ((pos.j+1)<size) && 
      (maze[pos.i][pos.j+1] == 0);
    bool isleft = ((pos.j-1)>=0) &&
      (maze[pos.i][pos.j-1] == 0);
    bool isup = ((pos.i-1)>=0) &&
      (maze[pos.i-1][pos.j] == 0);
    bool isdown = ((pos.i+1)<size) && 
      (maze[pos.i+1][pos.j] == 0);
    int nexti[4]={pos.i,pos.i,pos.i,pos.i};
    int nextj[4]={pos.j,pos.j,pos.j,pos.j};
    int count = 0;
    if(isright){nextj[count++]++;}
    if(isleft){nextj[count++]--;}
    if(isup){nexti[count++]--;}
    if(isdown){nexti[count++]++;}
    if(0==count) return pos;
    int nextinx = rand() % count;
    pos.i = nexti[nextinx];
    pos.j = nextj[nextinx];
    return pos;
}

typedef struct MazeDataType MazeDataType;
struct MazeDataType {
  int* board;
  size_t width;
  int levels;
};

MazeDataType dummy_dumm_dumm;

bool flood_fill(size_t size, int maze[size][size],
		IVec2 pos, int src_val, int dst_val, int end_val){

  MazeDataType dummy_data = {
    .board = (int*)maze,
    .width = size,
    .levels = 4
  };
  
  if(src_val == dst_val)
    return false;
  if((pos.i < 0) ||
     (pos.j < 0) ||
     (pos.i >= size) ||
     (pos.j >= size))
    return false;

  if(maze[pos.i][pos.j] != src_val)
    return (end_val == maze[pos.i][pos.j]) ||
      (dst_val == maze[pos.i][pos.j]);

  maze[pos.i][pos.j] = dst_val;
  bool not_border = true;
  not_border = flood_fill(size, maze, (IVec2){.i=pos.i,.j=pos.j+1},
				src_val, dst_val, end_val) && not_border;
  not_border = flood_fill(size, maze, (IVec2){.i=pos.i,.j=pos.j-1},
				src_val, dst_val, end_val)&& not_border;
  not_border = flood_fill(size, maze, (IVec2){.i=pos.i+1,.j=pos.j},
				src_val, dst_val, end_val)&& not_border;
  not_border = flood_fill(size, maze, (IVec2){.i=pos.i-1,.j=pos.j},
				src_val, dst_val, end_val)&& not_border;
  if(!not_border)
    maze[pos.i][pos.j] = end_val;

  return true;
}

bool is_valid_pos(int i, int j, size_t size){
  return (i >= 0) && (j >= 0) && (i < (int)size) && (j < (int)size);
}

IVec2 get_next_maze_cell(size_t size, int maze[size][size], IVec2 cell){

  IVec2 res = {-1,-1};
  if(!is_valid_pos(cell.i,cell.j,size))
    return res;
  if(3 != maze[cell.i][cell.j])
    return res;

  //Find what's on each side
  int vals[4];
  IVec2 pos[4] = {
    {cell.i,cell.j+1},
    {cell.i,cell.j-1},
    {cell.i+1,cell.j},
    {cell.i-1,cell.j}
  };
  for(int i = 0; i < 4; ++i)
    vals[i] = (is_valid_pos(pos[i].i, pos[i].j, size)?maze[pos[i].i][pos[i].j]:-1);

  //Now for each 4 sides, if is wall, find what's on other side directly forward
  IVec2 candidates[4];
  int count = 0;
  for(int k = 0; k < 4; ++k){
    if((1 == vals[k]) &&
       is_valid_pos(cell.i + 2 * (pos[k].i - cell.i),
		    cell.j + 2 * (pos[k].j - cell.j),
		    size) &&
       (0 == maze[cell.i + 2 * (pos[k].i - cell.i)]
	[cell.j + 2 * (pos[k].j - cell.j)])){
      candidates[count++] = pos[k];
    } 
  }
  if(count == 0){
    maze[cell.i][cell.j] = 2;
    return res;
  }
  return candidates[rand() % count];
}

IVec2 make_one_more_maze(size_t size, int maze[size][size], IVec2 last_cell){
  //0 -> unexplored 1-> wall 2-> explored 3-> explored and is boundary

  IVec2 next_cell = get_next_maze_cell(size, maze, last_cell);
  if(is_valid_pos(next_cell.i, next_cell.j, size)){
    IVec2 another_cell = {
      .i=last_cell.i + 2 * (next_cell.i - last_cell.i),
      .j=last_cell.j + 2 * (next_cell.j - last_cell.j)
    };
    maze[next_cell.i][next_cell.j] = 3;
    maze[another_cell.i][another_cell.j] = 3;
    return another_cell;
  }

  int count = 0;
  for(int i = 0; i < size; ++i){
    for(int j = 0; j < size; ++j){
      last_cell.i = i; last_cell.j = j;
      next_cell = get_next_maze_cell(size, maze, last_cell);
      if(is_valid_pos(next_cell.i, next_cell.j, size)){
	count++;
      }
    }
  }
  if(count == 0)
    return (IVec2){-1,-1};
  count = rand() % count;
  for(int i = 0; i < size; ++i){
    for(int j = 0; j < size; ++j){
      last_cell.i = i; last_cell.j = j;
      next_cell = get_next_maze_cell(size, maze, last_cell);
      if(is_valid_pos(next_cell.i, next_cell.j, size)){
	if(0 != (count--)) continue;
	IVec2 another_cell = {
	  .i=last_cell.i + 2 * (next_cell.i - last_cell.i),
	  .j=last_cell.j + 2 * (next_cell.j - last_cell.j)
	};
	maze[next_cell.i][next_cell.j] = 3;
	maze[another_cell.i][another_cell.j] = 3;
	return another_cell;
      }
    }
  }
  assert(false);
  return (IVec2){-2,-2};
}

void reset_maze(size_t size, int maze[size][size], IVec2* last_cell){
  //fill
  for(int i = 0; i < size; ++i)
    for(int j = 0; j < size; ++j)
      maze[i][j] = 1;
  for(int i = 0; i < size; i+=2)
    for(int j = 0; j < size; j+=2)
      maze[i][j] = 0;
  maze[0][0] = 3;
  *last_cell=(IVec2){0,0};
}

int main(int argc, char* argv[]){
  rl_init_lib("raylib");
  rl_set_trace_log_level(LOG_WARNING);

  if(argc > 1){
    int seed = atol(argv[1]);
    if(seed == 0)
      seed = time(NULL);
    srand(seed);
  }
  else
    srand(13);
  int win_wid = 1200;
  int win_hei = 900;
  rl_init_window(win_wid, win_hei, "Make Maze");
  rl_set_target_f_p_s(60);
  rl_set_exit_key('Q');
  win_wid = rl_get_screen_width(); 
  win_hei = rl_get_screen_height();

  RlColor maze_cols[]= {
    SKYBLUE,
    DARKBLUE,
    YELLOW,
    PINK
  };
  
  enum{MAZE_SIZE = 45};
  int maze[MAZE_SIZE][MAZE_SIZE] = {0};
  IVec2 last_cell;
  reset_maze(MAZE_SIZE, maze, &last_cell);
  

  int cell_wid = 5*win_wid / (7*MAZE_SIZE);
  int cell_hei = 5*win_hei / (7*MAZE_SIZE);

  while(!rl_window_should_close()){
    IVec2 mpos = {
      .y = rl_get_mouse_y(),
      .x = rl_get_mouse_x()
    };
    
    //Find if mpos in any cell
    bool m_in_cell =
      (mpos.x >= win_wid/7) &&
      (mpos.y >= win_hei/7) &&
      (mpos.x < win_wid*6/7) &&
      (mpos.y < win_hei*6/7);

    IVec2 m_cell={-1,-1};
    if(m_in_cell){
      m_cell.y = (mpos.y - win_hei/7) / cell_hei;
      m_cell.x = (mpos.x - win_wid/7) / cell_wid;
    }

    if(m_in_cell){
      if(rl_is_mouse_button_down(MOUSE_BUTTON_LEFT))
	maze[m_cell.i][m_cell.j] = 1;
      if(rl_is_mouse_button_down(MOUSE_BUTTON_RIGHT))
	maze[m_cell.i][m_cell.j] = 0;
      //flood_fill(MAZE_SIZE, maze, m_cell, maze[m_cell.i][m_cell.j], 2, 3);
    }
    
    if(rl_is_key_down(' ')){
      last_cell =  make_one_more_maze(MAZE_SIZE, maze, last_cell);
    }
    if(rl_is_key_pressed('R')){
      reset_maze(MAZE_SIZE, maze, &last_cell);
    }
    
    rl_begin_drawing();
    rl_clear_background(RAYWHITE);
    for(int i=0;i<MAZE_SIZE;++i){
      for(int j=0;j<MAZE_SIZE;++j){
	rl_draw_rectangle(win_wid/7 + j * cell_wid,
			  win_hei/7 + i * cell_hei,
			  cell_wid, cell_hei,
			  maze_cols[maze[i][j]]);
      }
    }
    if(m_in_cell){
      rl_draw_rectangle_lines(win_wid/7 + cell_wid * m_cell.x,
			      win_hei/7 + cell_hei * m_cell.y,
			      cell_wid, cell_hei,
			      BLACK);
    }

    rl_draw_text("Hello sweetie", win_wid*3/7, cell_hei, 25, MAGENTA);
        
    rl_end_drawing();
  }
  rl_close_window();
  rl_free_lib();
  return 0;
}
