!===============================================================================
! Fortran-C 互操作接口模块
! 
! 功能: 定义Fortran与C/C++函数的接口
! 说明: 使用iso_c_binding实现安全的语言间调用
!===============================================================================

module zipbomb_interfaces
    use iso_c_binding
    implicit none
    
    ! 公开接口
    public :: create_zipbomb, get_file_size, cleanup_resources
    
    ! C/C++函数接口声明
    interface
        
        !-----------------------------------------------------------------------
        ! C++函数: 创建ZIP炸弹
        ! 参数: filename - 输出文件名（C字符串）
        !-----------------------------------------------------------------------
        subroutine create_zipbomb(filename) bind(C, name="create_zipbomb")
            use iso_c_binding
            character(kind=c_char), intent(in) :: filename(*)
        end subroutine create_zipbomb
        
        !-----------------------------------------------------------------------
        ! C函数: 获取文件大小
        ! 参数: filename - 文件名（C字符串）
        ! 返回: 文件大小（字节），失败返回-1
        !-----------------------------------------------------------------------
        function get_file_size(filename) bind(C, name="get_file_size") result(size)
            use iso_c_binding
            character(kind=c_char), intent(in) :: filename(*)
            integer(c_long) :: size
        end function get_file_size
        
        !-----------------------------------------------------------------------
        ! C函数: 检查文件是否存在
        ! 参数: filename - 文件名（C字符串）
        ! 返回: 1存在，0不存在
        !-----------------------------------------------------------------------
        function file_exists_c(filename) bind(C, name="file_exists") result(exists)
            use iso_c_binding
            character(kind=c_char), intent(in) :: filename(*)
            integer(c_int) :: exists
        end function file_exists_c
        
        !-----------------------------------------------------------------------
        ! C函数: 删除文件
        ! 参数: filename - 文件名（C字符串）
        ! 返回: 0成功，-1失败
        !-----------------------------------------------------------------------
        function delete_file_c(filename) bind(C, name="delete_file") result(result)
            use iso_c_binding
            character(kind=c_char), intent(in) :: filename(*)
            integer(c_int) :: result
        end function delete_file_c
        
        !-----------------------------------------------------------------------
        ! C++函数: 清理资源
        ! 说明: 清理C++分配的内存和临时文件
        !-----------------------------------------------------------------------
        subroutine cleanup_resources() bind(C, name="cleanup_resources")
            use iso_c_binding
        end subroutine cleanup_resources
        
        !-----------------------------------------------------------------------
        ! C++函数: 设置压缩参数（可选功能）
        ! 参数: 
        !   target_size - 目标解压大小（MB）
        !   compression_level - 压缩级别（1-9）
        !-----------------------------------------------------------------------
        subroutine set_compression_params(target_size, compression_level) &
            bind(C, name="set_compression_params")
            use iso_c_binding
            integer(c_int), value, intent(in) :: target_size
            integer(c_int), value, intent(in) :: compression_level
        end subroutine set_compression_params
        
    end interface
    
contains

    !---------------------------------------------------------------------------
    ! Fortran包装函数: 检查文件是否存在
    ! 提供更友好的Fortran接口
    !---------------------------------------------------------------------------
    function file_exists_fortran(filename) result(exists)
        character(len=*), intent(in) :: filename
        logical :: exists
        character(len=:), allocatable :: c_filename
        integer(c_int) :: c_result
        
        ! 转换为C兼容字符串
        c_filename = trim(filename) // c_null_char
        c_result = file_exists_c(c_filename)
        exists = (c_result == 1)
    end function file_exists_fortran
    
    !---------------------------------------------------------------------------
    ! Fortran包装函数: 删除文件
    ! 提供更友好的Fortran接口
    !---------------------------------------------------------------------------
    function delete_file_fortran(filename) result(success)
        character(len=*), intent(in) :: filename
        logical :: success
        character(len=:), allocatable :: c_filename
        integer(c_int) :: c_result
        
        ! 转换为C兼容字符串
        c_filename = trim(filename) // c_null_char
        c_result = delete_file_c(c_filename)
        success = (c_result == 0)
    end function delete_file_fortran
    
    !---------------------------------------------------------------------------
    ! Fortran包装函数: 获取人类可读的文件大小
    ! 自动转换为合适的单位（B, KB, MB, GB）
    !---------------------------------------------------------------------------
    function get_human_readable_size(filename) result(size_string)
        character(len=*), intent(in) :: filename
        character(len=32) :: size_string
        character(len=:), allocatable :: c_filename
        integer(c_long) :: size_bytes
        real :: size_real
        
        ! 获取文件大小
        c_filename = trim(filename) // c_null_char
        size_bytes = get_file_size(c_filename)
        
        if (size_bytes < 0) then
            size_string = "文件不存在"
        else if (size_bytes < 1024) then
            write(size_string, '(I0,A)') size_bytes, " B"
        else if (size_bytes < 1024*1024) then
            size_real = real(size_bytes) / 1024.0
            write(size_string, '(F6.2,A)') size_real, " KB"
        else if (size_bytes < 1024*1024*1024) then
            size_real = real(size_bytes) / (1024.0*1024.0)
            write(size_string, '(F6.2,A)') size_real, " MB"
        else
            size_real = real(size_bytes) / (1024.0*1024.0*1024.0)
            write(size_string, '(F6.2,A)') size_real, " GB"
        end if
    end function get_human_readable_size
    
end module zipbomb_interfaces