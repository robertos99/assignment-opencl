__kernel void vectorAdd( __global const int* a,
 __global const int* b,
 __global int* c,
const int size )
{
 int i = get_local_size( 0 ) * get_group_id( 0 ) + get_local_id( 0 );
 if ( i < size )
 {
 c[i] = a[i] + b[i];
 }
}