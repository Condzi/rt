; Function compile flags: /Ogtpy
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
;	COMDAT ?hit_BVH_internal@rt@@YA?AV?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@PEAUBVH_Node@1@AEBTVec3@1@1@Z
_TEXT	SEGMENT
$T1 = 32
current$ = 40
__$ReturnUdt$ = 128
$T2 = 136
root$ = 136
ray_origin$ = 144
ray_inv_dir$ = 152
?hit_BVH_internal@rt@@YA?AV?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@PEAUBVH_Node@1@AEBTVec3@1@1@Z PROC ; rt::hit_BVH_internal, COMDAT

; 88   : hit_BVH_internal(BVH_Node *root, Vec3 const &ray_origin, Vec3 const &ray_inv_dir) {

$LN35:
	mov	QWORD PTR [rsp+32], r9
	mov	QWORD PTR [rsp+24], r8
	mov	QWORD PTR [rsp+8], rcx
	push	rbx
	push	rbp
	push	rsi
	push	rdi
	push	r12
	push	r13
	push	r14
	push	r15
	sub	rsp, 56					; 00000038H
	mov	r10, r9
	mov	r11, r8
	mov	rdi, rdx
	mov	r14, rcx
	xor	esi, esi
	mov	DWORD PTR $T1[rsp], esi
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector

; 398  :     _CONSTEXPR20 _Vector_val() noexcept : _Myfirst(), _Mylast(), _Myend() {}

	mov	QWORD PTR [rcx], rsi
	mov	QWORD PTR [rcx+8], rsi
	mov	QWORD PTR [rcx+16], rsi
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx

; 131  :   return candidates;

	mov	DWORD PTR $T1[rsp], 1

; 89   :   std::vector<BVH_Node *> candidates;
; 90   :   BVH_Node               *current     = root;

	mov	QWORD PTR current$[rsp], rdx

; 91   :   BVH_Node               *lastVisited = nullptr;

	test	rdx, rdx

; 92   : 
; 93   :   while (current != nullptr) {

	je	$LN32@hit_BVH_in

; 94   :     if (!ray_vs_aabb(ray_origin,

	vmovss	xmm0, DWORD PTR __real@3a83126f
	vmovss	DWORD PTR $T2[rsp], xmm0
	vmovss	xmm1, DWORD PTR __real@7f7fffff
	vmovss	DWORD PTR $T2[rsp+4], xmm1
	mov	rax, rdx
	mov	rbp, rdx
	mov	r15, rdx
	mov	r12, rdx
	mov	r13, rdx
	mov	rbx, QWORD PTR $T2[rsp]
	npad	5
$LL2@hit_BVH_in:
	lea	r9, QWORD PTR [rax+24]
	mov	r8, rbx
	mov	rdx, r10
	mov	rcx, r11
	call	?ray_vs_aabb@rt@@YA_NAEIBTVec3@1@0TVec2@1@AEIBTAABB@1@@Z ; rt::ray_vs_aabb
	test	al, al
	jne	SHORT $LN4@hit_BVH_in

; 95   :                      ray_inv_dir,
; 96   :                      {.min = 0.001f, .max = FLT_MAX},
; 97   :                      current->aabb)) {
; 98   :       // backtrack to parent
; 99   :       lastVisited = current;

	mov	rsi, rbp

; 100  :       current     = current->parent;

	mov	rdi, QWORD PTR [rbp+16]

; 101  :       continue;

	jmp	SHORT $LN29@hit_BVH_in
$LN4@hit_BVH_in:

; 102  :     }
; 103  : 
; 104  :     // If we reach a leaf node, it's a potential hit
; 105  :     if (current->left == nullptr && current->right == nullptr) {

	mov	rax, QWORD PTR [rdi]
	test	rax, rax
	jne	SHORT $LN5@hit_BVH_in
	cmp	QWORD PTR [r15+8], rax
	jne	SHORT $LN5@hit_BVH_in
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector

; 781  :         if (_Mylast != _My_data._Myend) {

	mov	rdx, QWORD PTR [r14+8]
	cmp	rdx, QWORD PTR [r14+16]
	je	SHORT $LN20@hit_BVH_in

; 797  :             _Construct_in_place(*_Mylast, _STD forward<_Valty>(_Val)...);

	mov	QWORD PTR [rdx], rdi

; 798  :         } else {
; 799  :             _ASAN_VECTOR_EXTEND_GUARD(static_cast<size_type>(_Mylast - _My_data._Myfirst) + 1);
; 800  :             _Alty_traits::construct(_Getal(), _Unfancy(_Mylast), _STD forward<_Valty>(_Val)...);
; 801  :             _ASAN_VECTOR_RELEASE_GUARD;
; 802  :         }
; 803  : 
; 804  :         _Orphan_range(_Mylast, _Mylast);
; 805  :         _Ty& _Result = *_Mylast;
; 806  :         ++_Mylast;

	add	QWORD PTR [r14+8], 8
	mov	rsi, r12
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx

; 110  :       current     = current->parent;

	mov	rdi, QWORD PTR [r12+16]

; 111  :       continue;

	jmp	SHORT $LN29@hit_BVH_in
$LN20@hit_BVH_in:
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector

; 785  :         return *_Emplace_reallocate(_Mylast, _STD forward<_Valty>(_Val)...);

	lea	r8, QWORD PTR current$[rsp]
	mov	rcx, r14
	call	??$_Emplace_reallocate@AEBQEAUBVH_Node@rt@@@?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@AEAAPEAPEAUBVH_Node@rt@@QEAPEAU23@AEBQEAU23@@Z ; std::vector<rt::BVH_Node *,std::allocator<rt::BVH_Node *> >::_Emplace_reallocate<rt::BVH_Node * const &>
	mov	rsi, r13
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx

; 110  :       current     = current->parent;

	mov	rdi, QWORD PTR [r13+16]

; 111  :       continue;

	jmp	SHORT $LN29@hit_BVH_in
$LN5@hit_BVH_in:

; 112  :     }
; 113  : 
; 114  :     // If we haven't visited the left child yet, visit it
; 115  :     if (lastVisited != current->left && lastVisited != current->right) {

	cmp	rsi, rax
	je	SHORT $LN6@hit_BVH_in
	cmp	rsi, QWORD PTR [rdi+8]
	je	SHORT $LN6@hit_BVH_in

; 116  :       current = current->left;

	mov	rdi, rax

; 117  :       continue;

	jmp	SHORT $LN29@hit_BVH_in
$LN6@hit_BVH_in:

; 118  :     }
; 119  : 
; 120  :     // If we haven't visited the right child yet, visit it
; 121  :     if (lastVisited != current->right) {

	mov	rax, QWORD PTR [rdi+8]
	cmp	rsi, rax
	je	SHORT $LN7@hit_BVH_in

; 122  :       current = current->right;

	mov	rdi, rax

; 123  :       continue;

	jmp	SHORT $LN29@hit_BVH_in
$LN7@hit_BVH_in:

; 124  :     }
; 125  : 
; 126  :     // If both children have been visited, backtrack to parent
; 127  :     lastVisited = current;

	mov	rsi, rdi

; 128  :     current     = current->parent;

	mov	rdi, QWORD PTR [rdi+16]
$LN29@hit_BVH_in:

; 92   : 
; 93   :   while (current != nullptr) {

	mov	rax, rdi
	mov	QWORD PTR current$[rsp], rdi
	mov	rbp, rdi
	mov	r15, rdi
	mov	r12, rdi
	mov	r13, rdi
	test	rdi, rdi
	mov	r10, QWORD PTR ray_inv_dir$[rsp]
	mov	r11, QWORD PTR ray_origin$[rsp]
	jne	$LL2@hit_BVH_in
$LN32@hit_BVH_in:

; 132  : }

	mov	rax, r14
	add	rsp, 56					; 00000038H
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rdi
	pop	rsi
	pop	rbp
	pop	rbx
	ret	0
	int	3
?hit_BVH_internal@rt@@YA?AV?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@PEAUBVH_Node@1@AEBTVec3@1@1@Z ENDP ; rt::hit_BVH_internal
_TEXT	ENDS
