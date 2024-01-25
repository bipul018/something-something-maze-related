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


#define ifelse1(func, p1, e1)\
  if((func)(p1)) e1

#define ifelse2(func, p1, e1, ...)	\
  ifelse1(func,p1,e1) else ifelse1(func, __VA_ARGS__)

#define ifelse3(func, p1, e1, ...)			\
  ifelse1(func,p1,e1) else ifelse2(func, __VA_ARGS__)

#define ifelse4(func, p1, e1, ...)	\
  ifelse1(func,p1,e1) else ifelse3(func, __VA_ARGS__)


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
IVec2 add_maze_cell_bfs(size_t size, int maze[size][size]){
  IVec2 next_cell ;
  IVec2 last_cell;
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
IVec2 add_maze_cell_dfs(size_t size, int maze[size][size], IVec2 last_cell){
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
  return (IVec2){-1,-1};
  //return add_maze_cell_bfs(size, maze);
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

enum{
  MAX_PATH_FACTOR = 10
};

void init_dum_maze_data(size_t size,const int og_maze[size][size], int dum_maze[size][size], IVec2* init_pos, IVec2* init_dir){
  init_pos->i = init_pos->j = 0;
  for(size_t i = 0; i < size; ++i)
    for(size_t j = 0; j < size; ++j)
      dum_maze[i][j] = 3;
  dum_maze[0][0] = 2;

  //Find dir by og maze data
  if(1 != og_maze[1][0])
    *init_dir = (IVec2){.i = 1, .j = 0};
  else
    *init_dir = (IVec2){.i = 0, .j = 1};
}

size_t fill_path_by_dum_maze(size_t size, int maze[size][size],
			     int path[size * size]){

  IVec2 cell = {0,0};
  IVec2 dir ;
  if(1 != maze[1][0])
    dir=(IVec2){.i = 1, .j = 0};
  else
    dir=(IVec2){.i = 0, .j = 1};
  size_t path_len = 1;
  path[0] = 0;
  
  while(path_len < size*size){
    int inx = 0;
    {
      IVec2 rot = {
	.x = dir.y,
	.y = -dir.x,
      };
      dir = rot;
    }
    for(;inx < 3; ++inx){
    
      IVec2 adj = {
	.x = cell.x + dir.x,
	.y = cell.y + dir.y
      };

      IVec2 rot = {
	.x = -dir.y,
	.y = dir.x
      };

      if(is_valid_pos(adj.i, adj.j, size)&&
	 (2 == maze[adj.i][adj.j])){
	cell = adj;
	path[path_len++] = adj.j + adj.i * size;
	break;
      }
      dir = rot;
    }
    if(inx == 3)
      break;
  } 
  return path_len;
}

bool move_one_step_in_dum_maze(size_t size, int dum_maze[size][size],
			       IVec2* ppos,IVec2* pdir, int og_maze[size][size]){
  //In the og_maze, 0 and 3 are used in maze making step only
  // 1 means wall, 2 means path

  //In this dum_maze, we assign 1 and 2 to processed and 0 and 3 to unprocessed
  //1 is wall or unreachable path
  //2 is visited path, which can have at most two adjacents at a time
  //0 means there is a path but it is not processed 3 means we don't know anything

  //Need to handle first case, where dir is 0,0 later
  IVec2 dir;
  if(!(pdir->i || pdir->j)){
    assert(false);
    dir = (IVec2){
      1,0
    };
  }
  dir = *pdir;
  //Rotate -90 first
  {
    IVec2 rot = {.x = dir.y, .y = -dir.x};
    dir = rot;
  }

  //3 dirs for simulating 'robot', for this just use predefined one of 3
  //'left' 'straight' and 'right'

  int inx = 0;
  for(; inx < 3; ++inx){
 
    IVec2 next_pos = {
      .x = ppos->x + dir.x,
      .y = ppos->y + dir.y,
    };
    if(is_valid_pos(next_pos.i, next_pos.j, size)){
      //Query data from og maze if not available
      if(3 == dum_maze[next_pos.i][next_pos.j]){
	dum_maze[next_pos.i][next_pos.j] = og_maze[next_pos.i][next_pos.j];
	if(1 != dum_maze[next_pos.i][next_pos.j])
	  dum_maze[next_pos.i][next_pos.j] = 0;
      }
      if(0 == dum_maze[next_pos.i][next_pos.j]){
	//This is it, move here
	dum_maze[next_pos.i][next_pos.j] = 2;
	*pdir = dir;
	*ppos = next_pos;
	return true;
      }
      if(2 == dum_maze[next_pos.i][next_pos.j]){
	//This happens when backtracking
	dum_maze[next_pos.i][next_pos.j] = 2;
	dum_maze[ppos->i][ppos->j] = 1;
	*pdir = dir;
	*ppos = next_pos;
	return true;
      }
    }
    
    IVec2 rot = {.x = -dir.y, .y = dir.x};
    dir = rot;
       
  }

  if(3 == inx){
    //Handle backtracking
    //Mark current pos unreachable
    dum_maze[ppos->i][ppos->j] = 1;
    //Reverse direction
    pdir->i = -pdir->i;
    pdir->j = -pdir->j;
    //Set pos to this
    ppos->i += pdir->i;
    ppos->j += pdir->j;
  }
  
  return true;
}



size_t solve_one_hand_on_wall(size_t size, int maze[size][size], int path[MAX_PATH_FACTOR * size*size], size_t path_len){
  if(size < 2)
    return path_len;
    
  if(0 == path_len){
    path_len++;
    path[0] = 0;
  }
    

  IVec2 c1,c2;
  int curr_val;
  if(1 == path_len){
    c2 = (IVec2){
      .i = path[path_len-1] / size,
      .j = path[path_len-1] % size
    };
    c1 = (IVec2){
      .i = c2.i-1,
      .j = c2.j
    };
  }
  else{ 
    c1 = (IVec2){
      .i = path[path_len-2] / size,
      .j = path[path_len-2] % size
    };
    c2 = (IVec2){
      .i = path[path_len-1] / size,
      .j = path[path_len-1] % size
    };
  }

  //Collect neighbours
  curr_val = maze[c2.i][c2.j];

  //Now begin rotating c1 -> c2
  for(int i = 0; i < 4; ++i){
    //Difference
    IVec2 diff = {
      .x = c2.x - c1.x,
      .y = c2.y - c1.y
    };
    //Rotate by 90
    IVec2 rot = {
      .x = -diff.y,
      .y = diff.x
    };
    //Add to c1
    c1.x = c2.x + rot.x;
    c1.y = c2.y + rot.y;
    //Find if c1 is suitable
    if(is_valid_pos(c1.i, c1.j, size) &&
       (curr_val == maze[c1.i][c1.j])){
      path[path_len] = c1.j + c1.i * size;
      path_len++;
      break;
    }
  }
  return path_len;
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
  
  enum{MAZE_SIZE = 31};
  int maze[MAZE_SIZE][MAZE_SIZE] = {0};
  int dum_maze[MAZE_SIZE][MAZE_SIZE] = {0};
  IVec2 dum_maze_cell = {0};
  IVec2 dum_maze_dir = {0};
  IVec2 last_cell;
  
  reset_maze(MAZE_SIZE, maze, &last_cell);
  bool dum_maze_ready = false;
  //init_dum_maze_data(MAZE_SIZE,maze, dum_maze, &dum_maze_cell, &dum_maze_dir);

  int cell_wid = 5*win_wid / (7*MAZE_SIZE);
  int cell_hei = 5*win_hei / (7*MAZE_SIZE);

  int curr_path[MAX_PATH_FACTOR * MAZE_SIZE * MAZE_SIZE] = {0};
  size_t curr_path_len=1;
  

  int turn_counter = 0;
  
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
      ifelse4(rl_is_key_released,
	      '0', maze[m_cell.i][m_cell.j] = 0;,
	      '1', maze[m_cell.i][m_cell.j] = 1;,
	      '2', maze[m_cell.i][m_cell.j] = 2;,
	      '3', maze[m_cell.i][m_cell.j] = 3;);
    }

    if(curr_path_len < _countof(curr_path)){
      int inx = curr_path[curr_path_len-1];
      int i = inx / MAZE_SIZE;
      int j = inx % MAZE_SIZE;
    
      if(rl_is_key_pressed(KEY_RIGHT) &&
	 (j < (MAZE_SIZE -1))){
	j++;
	inx = i * MAZE_SIZE + j;
	curr_path[curr_path_len++] = inx;
      }
      
      if(rl_is_key_pressed(KEY_LEFT) &&
	 (j > 0)){
	j--;
	inx = i * MAZE_SIZE + j;
	curr_path[curr_path_len++] = inx;
      }	
	
      if(rl_is_key_pressed(KEY_DOWN) &&
	 (i < (MAZE_SIZE -1))){
	i++;
	inx = i * MAZE_SIZE + j;
	curr_path[curr_path_len++] = inx;
      }
      
      if(rl_is_key_pressed(KEY_UP) &&
	 (i > 0)){
	i--;
	inx = i * MAZE_SIZE + j;
	curr_path[curr_path_len++] = inx;
      }
    }
      
    
    if(rl_is_key_down(' ')){
      if(turn_counter < 8)
	last_cell =  add_maze_cell_dfs(MAZE_SIZE, maze, last_cell);
      else
	last_cell = add_maze_cell_bfs(MAZE_SIZE, maze);
      turn_counter = (turn_counter+1)%9;
    }
    if(rl_is_key_pressed('R')){
      reset_maze(MAZE_SIZE, maze, &last_cell);
      curr_path_len = 1;
      dum_maze_ready = false;

    }
    if(rl_is_key_pressed('P')){
      curr_path_len = 1;
    }
    if(rl_is_key_down('H')){
      curr_path_len = solve_one_hand_on_wall(MAZE_SIZE, maze,curr_path, curr_path_len);
    }
    if(rl_is_key_released('D')){
      if(!dum_maze_ready)
	init_dum_maze_data(MAZE_SIZE,maze, dum_maze, &dum_maze_cell, &dum_maze_dir);
      dum_maze_ready = true;
      move_one_step_in_dum_maze(MAZE_SIZE, dum_maze,
				&dum_maze_cell, &dum_maze_dir,
				maze);
      curr_path_len = fill_path_by_dum_maze(MAZE_SIZE, dum_maze, curr_path);
    }
    if(rl_is_key_pressed('F')){
      dum_maze_ready = false;
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

    //Draw path
    for(int i = 0; i < (int)curr_path_len-1; ++i){
      IVec2 src={
	.i = curr_path[i] / MAZE_SIZE,
	.j = curr_path[i] % MAZE_SIZE,
      };
      IVec2 dst={
	.i = curr_path[i+1] / MAZE_SIZE,
	.j = curr_path[i+1] % MAZE_SIZE,
      };
      src.x = win_wid/7 + cell_wid * (src.x + 0.5f);
      src.y = win_hei/7 + cell_hei * (src.y + 0.5f);
      dst.x = win_wid/7 + cell_wid * (dst.x + 0.5f);
      dst.y = win_hei/7 + cell_hei * (dst.y + 0.5f);
      rl_draw_line(src.x,src.y,dst.x, dst.y, BLACK);
    }
    if(curr_path_len > 0){
      IVec2 head = {
	.i = curr_path[curr_path_len-1] / MAZE_SIZE,
	.j = curr_path[curr_path_len-1] % MAZE_SIZE
      };
      head.x = win_wid/7 + cell_wid * (head.x + 0.5f);
      head.y = win_hei/7 + cell_hei * (head.y + 0.5f);
      rl_draw_circle(head.x , head.y, (cell_wid + cell_hei)/4, RED);
    }
      
    rl_draw_text("Hello sweetie", win_wid*3/7, cell_hei, 25, MAGENTA);

    
    rl_end_drawing();
  }
  rl_close_window();
  rl_free_lib();
  return 0;
}
