typedef struct vmapnode{
 value v;
 value next;
}vmapnode;

#define VAR_MAP_LEN 108

typedef struct vmap{
 vmapnode map[108];
 long count;
}vmap;