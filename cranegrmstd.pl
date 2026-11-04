$combine["..\cranestd\math.ce"]::

$int a = 10;
$print("a=" , a , 90);
$int b = $rand(0..103);

-- section forever value def.
$fer $flo P 3.14

-- function implement.
$fnc__[add][int a, int b]:$back a + b;:

-- package condition instruction.
$pack__[p1] a <= 130 or a < P and a - 100 < 10;
$pack__[p2] a < (213 / 12) and a <= 2123;

-- origin using bulid-in function.
$loop__[p1]: print("all=", l1, a); $loop__: [p2] $sadd[a];::

-- if condition judge.
$if__[a < (213 / 12) and a <= 2123]: $print("");:

-- package union.
$union[MachineE]:
  $long id; 
  $float price;
  $string machine_name;
:

-- use union data.
MachineE me = {10, 103.2, "PYR#41"};
$print(me->id);

$fn[test1][string s, MachineE me][static][const]:
  -- ...
  $back -- ..
:

$fer $float MS_E 190.214
$Cgetf["..\machine_study.c\{study_main}"][178, MS_E]=> me;

$print($json[me]);

-- funtion use

$int all = $ansfnc[add][a, b]::

$combine["..\cranestd\math.ce"]::
$combine["..\cranestd\io.ce"]::

$fer $float TAX_RATE 0.08
$int global_counter = 0;

$union[MachineE]:
  $long id; 
  $float price;
  $string machine_name;
:

$struct[Order]:
  $int order_id;
  $float base_price;
  $MachineE machine_data;
:

$fnc__[calc_final_price][float base, int quantity]:
  $float sub_total = base * quantity;
  $float tax = sub_total * TAX_RATE;
  $float final_price = sub_total + tax;
  $back final_price;
:

$fnc__[process_machine][MachineE me, int max_loops]:
  $int local_i = 0;
  
  $loop__[L1]: 
    $if__[local_i >= max_loops]: 
      $loop__:; 
    :
    $print("Machine ID: ", me->id, " | Name: ", me->machine_name);
    local_i = local_i + 1;
  $loop__: [L1] ::
  
  $back local_i;
:

$int a = 10;
$int b = $rand(0..103);
$print("Random B generated: ", b);

Order my_order = {9527, 100.0, {8888, 99.9, "Crane_Engine_01"}};

$float total = $ansfnc[calc_final_price][my_order.base_price, 5]::
$print("Total Price: ", total);

$int loops = $ansfnc[process_machine][my_order.machine_data, 3]::
$print("Loops executed: ", loops);

$pack__[is_premium] a <= 130 or a < b and total > 500;
$pack__[is_basic] a < (213 / 12) and b <= 2123;

$if__[is_premium]: 
  $print("User is PREMIUM.");
: $if__[is_basic]:
  $print("User is BASIC.");
: $if__[not is_premium and not is_basic]:
  $print("User is UNKNOWN.");
:

$fer $float MS_E 190.214
$Cgetf["..\machine_study.c\{study_main}"][loops, MS_E] => my_order;

$print("Order JSON: ", $json[my_order]);

$union[GlobalContext]:
  $int total_orders;
  $float total_revenue;
:

GlobalContext ctx = {1, total};
$print("Context Revenue: ", ctx->total_revenue);