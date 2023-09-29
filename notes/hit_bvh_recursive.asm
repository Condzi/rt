; Function compile flags: /Ogtpy
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx
;	COMDAT ?hit_BVH_internal@rt@@YAXPEAUBVH_Node@1@AEBTVec3@1@1@Z
_TEXT	SEGMENT
root$ = 48
ray_origin$ = 56
ray_inv_dir$ = 64
$T1 = 72
?hit_BVH_internal@rt@@YAXPEAUBVH_Node@1@AEBTVec3@1@1@Z PROC ; rt::hit_BVH_internal, COMDAT

; 88   : hit_BVH_internal(BVH_Node *root, Vec3 const &ray_origin, Vec3 const &ray_inv_dir) {

$LN28:
	mov	QWORD PTR [rsp+16], rbx
	mov	QWORD PTR [rsp+24], rsi
	mov	QWORD PTR [rsp+8], rcx
	push	rdi
	sub	rsp, 32					; 00000020H
	mov	rdi, r8

; 89   :   if (!ray_vs_aabb(

	mov	DWORD PTR $T1[rsp], 981668463		; 3a83126fH
	mov	rsi, rdx
	mov	DWORD PTR $T1[rsp+4], 2139095039	; 7f7fffffH
	mov	r8, QWORD PTR $T1[rsp]
	lea	r9, QWORD PTR [rcx+16]
	mov	rbx, rcx
	mov	rdx, rdi
	mov	rcx, rsi
	call	?ray_vs_aabb@rt@@YA_NAEIBTVec3@1@0TVec2@1@AEIBTAABB@1@@Z ; rt::ray_vs_aabb
	test	al, al
	je	$LN22@hit_BVH_in
	npad	7
$LL14@hit_BVH_in:

; 90   :           ray_origin, ray_inv_dir, {.min = 0.001f, .max = FLT_MAX}, root->aabb)) {
; 91   :     return;
; 92   :   }
; 93   : 
; 94   :   // Potential hit
; 95   :   if (root->left == NULL && root->right == NULL) {

	mov	rcx, QWORD PTR [rbx]
	test	rcx, rcx
	jne	SHORT $LN3@hit_BVH_in
	cmp	QWORD PTR [rbx+8], rcx
	je	SHORT $LN18@hit_BVH_in
$LN3@hit_BVH_in:

; 97   :     return;
; 98   :   }
; 99   : 
; 100  :   hit_BVH_internal(root->left, ray_origin, ray_inv_dir);

	mov	r8, rdi
	mov	rdx, rsi
	call	?hit_BVH_internal@rt@@YAXPEAUBVH_Node@1@AEBTVec3@1@1@Z ; rt::hit_BVH_internal

; 101  :   hit_BVH_internal(root->right, ray_origin, ray_inv_dir);

	mov	r9, QWORD PTR [rbx+8]
	mov	rdx, rdi
	mov	rbx, r9
	mov	DWORD PTR $T1[rsp], 981668463		; 3a83126fH
	mov	DWORD PTR $T1[rsp+4], 2139095039	; 7f7fffffH
	add	r9, 16
	mov	r8, QWORD PTR $T1[rsp]
	mov	rcx, rsi
	mov	QWORD PTR root$[rsp], rbx
	call	?ray_vs_aabb@rt@@YA_NAEIBTVec3@1@0TVec2@1@AEIBTAABB@1@@Z ; rt::ray_vs_aabb
	test	al, al
	jne	SHORT $LL14@hit_BVH_in

; 102  : }

	mov	rbx, QWORD PTR [rsp+56]
	mov	rsi, QWORD PTR [rsp+64]
	add	rsp, 32					; 00000020H
	pop	rdi
	ret	0
$LN18@hit_BVH_in:

; 96   :     candidates.push_back(root);

	mov	rax, QWORD PTR gs:88
	mov	ecx, DWORD PTR _tls_index
	mov	rdi, QWORD PTR [rax+rcx*8]
	mov	eax, OFFSET FLAT:__tls_guard
	cmp	BYTE PTR [rax+rdi], 0
	jne	SHORT $LN5@hit_BVH_in
	call	__dyn_tls_on_demand_init
$LN5@hit_BVH_in:
	mov	ecx, OFFSET FLAT:?candidates@rt@@3V?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@A ; rt::candidates
	add	rcx, rdi
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector

; 781  :         if (_Mylast != _My_data._Myend) {

	mov	rdx, QWORD PTR [rcx+8]
	cmp	rdx, QWORD PTR [rcx+16]
	je	SHORT $LN10@hit_BVH_in

; 797  :             _Construct_in_place(*_Mylast, _STD forward<_Valty>(_Val)...);

	mov	QWORD PTR [rdx], rbx

; 798  :         } else {
; 799  :             _ASAN_VECTOR_EXTEND_GUARD(static_cast<size_type>(_Mylast - _My_data._Myfirst) + 1);
; 800  :             _Alty_traits::construct(_Getal(), _Unfancy(_Mylast), _STD forward<_Valty>(_Val)...);
; 801  :             _ASAN_VECTOR_RELEASE_GUARD;
; 802  :         }
; 803  : 
; 804  :         _Orphan_range(_Mylast, _Mylast);
; 805  :         _Ty& _Result = *_Mylast;
; 806  :         ++_Mylast;

	add	QWORD PTR [rcx+8], 8
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx

; 102  : }

	mov	rbx, QWORD PTR [rsp+56]
	mov	rsi, QWORD PTR [rsp+64]
	add	rsp, 32					; 00000020H
	pop	rdi
	ret	0
$LN10@hit_BVH_in:
; File D:\Programs\vs-community-22\VC\Tools\MSVC\14.37.32822\include\vector

; 785  :         return *_Emplace_reallocate(_Mylast, _STD forward<_Valty>(_Val)...);

	lea	r8, QWORD PTR root$[rsp]
	call	??$_Emplace_reallocate@AEBQEAUBVH_Node@rt@@@?$vector@PEAUBVH_Node@rt@@V?$allocator@PEAUBVH_Node@rt@@@std@@@std@@AEAAPEAPEAUBVH_Node@rt@@QEAPEAU23@AEBQEAU23@@Z ; std::vector<rt::BVH_Node *,std::allocator<rt::BVH_Node *> >::_Emplace_reallocate<rt::BVH_Node * const &>
$LN22@hit_BVH_in:
; File W:\pwr\engineering-thesis\rt\code\cpu_rt\bvh.cxx

; 102  : }

	mov	rbx, QWORD PTR [rsp+56]
	mov	rsi, QWORD PTR [rsp+64]
	add	rsp, 32					; 00000020H
	pop	rdi
	ret	0
?hit_BVH_internal@rt@@YAXPEAUBVH_Node@1@AEBTVec3@1@1@Z ENDP ; rt::hit_BVH_internal
_TEXT	ENDS
