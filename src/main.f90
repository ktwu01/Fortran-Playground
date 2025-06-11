!===============================================================================
! Fortran ZIP炸弹生成器 - 主程序
!
! 功能: 演示Fortran与C/C++混合编程创建ZIP炸弹
! 作者: Fortran-Playground项目
! 警告: 仅用于教学目的，请勿恶意使用！
!===============================================================================

program zipbomb_generator
    use iso_c_binding
    use zipbomb_interfaces  ! 引入C接口模块
    implicit none

    ! 声明变量
    character(len=256) :: output_filename
    character(len=:), allocatable :: c_filename
    integer(c_long) :: file_size
    logical :: file_exists

    ! 程序开始信息
    call print_banner()
    call print_warning()

    ! 设置输出文件名
    output_filename = "bomb.zip"

    ! 检查文件是否已存在
    inquire(file=trim(output_filename), exist=file_exists)
    if (file_exists) then
        write(*,'(A)') "⚠️  文件已存在，将覆盖: " // trim(output_filename)
        write(*,'(A)') "   当前大小: " // get_human_readable_size(output_filename)
        write(*,'(A)', advance='no') "继续？ (y/N): "
        call confirm_overwrite()
    end if

    ! 转换为C兼容字符串
    c_filename = trim(output_filename) // c_null_char

    write(*,'(A)') "🔧 开始生成ZIP炸弹..."
    write(*,'(A)') "   目标文件: " // trim(output_filename)
    write(*,'(A)') "   预期压缩前大小: ~5KB"
    write(*,'(A)') "   预期解压后大小: ~10GB"
    write(*,*)

    ! 调用C++函数创建ZIP炸弹
    call create_zipbomb(c_filename)

    ! 检查生成结果
    inquire(file=trim(output_filename), exist=file_exists)
    if (file_exists) then
        ! 获取实际文件大小
        file_size = get_file_size(c_filename)

        write(*,'(A)') "✅ ZIP炸弹生成成功！"
        write(*,'(A)') "   文件名: " // trim(output_filename)
        write(*,'(A)') "   文件大小: " // get_human_readable_size(output_filename)
        write(*,*)

        call print_usage_warning()
        call print_test_instructions()
    else
        write(*,'(A)') "❌ 生成失败！请检查编译器和依赖库。"
        stop 1
    end if

    write(*,'(A)') "程序执行完毕。"

    ! 清理资源
    call cleanup_resources()

contains

    !---------------------------------------------------------------------------
    ! 打印程序横幅
    !---------------------------------------------------------------------------
    subroutine print_banner()
        write(*,'(A)') repeat("=", 70)
        write(*,'(A)') "           Fortran ZIP炸弹生成器 v1.0"
        write(*,'(A)') "        多语言协作教学演示项目"
        write(*,'(A)') "     (Fortran + C++ + C 混合编程示例)"
        write(*,'(A)') repeat("=", 70)
        write(*,*)
    end subroutine print_banner

    !---------------------------------------------------------------------------
    ! 打印安全警告
    !---------------------------------------------------------------------------
    subroutine print_warning()
        write(*,'(A)') "⚠️  **安全警告** ⚠️"
        write(*,'(A)') "   本工具仅用于教学和研究目的"
        write(*,'(A)') "   禁止用于恶意攻击网站或系统"
        write(*,'(A)') "   请在安全的测试环境中使用"
        write(*,'(A)') "   生成的文件可能消耗大量系统资源"
        write(*,*)
    end subroutine print_warning

    !---------------------------------------------------------------------------
    ! 确认覆盖现有文件
    !---------------------------------------------------------------------------
    subroutine confirm_overwrite()
        character(len=10) :: user_input

        read(*,'(A)') user_input
        if (trim(adjustl(user_input)) /= 'y' .and. trim(adjustl(user_input)) /= 'Y') then
            write(*,'(A)') "操作已取消。"
            stop 0
        end if
    end subroutine confirm_overwrite

    !---------------------------------------------------------------------------
    ! 打印使用警告
    !---------------------------------------------------------------------------
    subroutine print_usage_warning()
        write(*,'(A)') "⚠️  **使用注意事项**:"
        write(*,'(A)') "   1. 请勿在生产环境中解压此文件"
        write(*,'(A)') "   2. 解压前确保有足够的磁盘空间(>10GB)"
        write(*,'(A)') "   3. 解压可能消耗大量内存和CPU资源"
        write(*,'(A)') "   4. 建议在虚拟机或容器中测试"
        write(*,*)
    end subroutine print_usage_warning

    !---------------------------------------------------------------------------
    ! 打印测试说明
    !---------------------------------------------------------------------------
    subroutine print_test_instructions()
        write(*,'(A)') "🧪 **测试建议**:"
        write(*,'(A)') "   # 检查文件信息"
        write(*,'(A)') "   file bomb.zip"
        write(*,'(A)') "   ls -lh bomb.zip"
        write(*,*)
        write(*,'(A)') "   # 查看ZIP内容（安全）"
        write(*,'(A)') "   unzip -l bomb.zip"
        write(*,*)
        write(*,'(A)') "   # ⚠️ 谨慎解压测试（需要大量空间）"
        write(*,'(A)') "   # mkdir test_dir && cd test_dir"
        write(*,'(A)') "   # unzip ../bomb.zip"
        write(*,*)
    end subroutine print_test_instructions

end program zipbomb_generator
