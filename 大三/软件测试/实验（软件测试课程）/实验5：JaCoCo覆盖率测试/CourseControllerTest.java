package com.wdd.studentmanager.controller;

import com.wdd.studentmanager.domain.Course;
import com.wdd.studentmanager.service.CourseService;
import com.wdd.studentmanager.util.AjaxResult;
import com.wdd.studentmanager.util.Data;
import com.wdd.studentmanager.util.PageBean;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.springframework.http.MediaType;
import org.springframework.test.context.junit4.SpringRunner;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.test.web.servlet.MvcResult;
import org.springframework.test.web.servlet.setup.MockMvcBuilders;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.junit.Assert.*;
import static org.mockito.ArgumentMatchers.*;
import static org.mockito.Mockito.*;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.result.MockMvcResultHandlers.print;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

/**
 * CourseController 单元测试类
 * 使用 MockMvc + Mockito 对课程管理模块进行覆盖率测试
 * 不依赖真实数据库，通过 Mock 对象模拟 Service 层行为
 *
 * 测试覆盖方法：
 *  1. courseList()              - 返回课程列表页面视图
 *  2. getClazzList(...)         - 分页查询课程列表（含 combox 分支、普通分支）
 *  3. addCourse(Course)         - 添加课程（成功/失败/异常三条路径）
 *  4. editCourse(Course)        - 修改课程（成功/失败/异常三条路径）
 *  5. deleteCourse(Data)        - 删除课程（成功/失败/异常三条路径）
 */
@RunWith(SpringRunner.class)
public class CourseControllerTest {

    /** 被测 Controller，由 Mockito 注入 Mock 的 Service */
    @InjectMocks
    private CourseController courseController;

    /** Mock 掉 CourseService，避免连接真实数据库 */
    @Mock
    private CourseService courseService;

    /** Spring MVC 测试框架，模拟 HTTP 请求 */
    private MockMvc mockMvc;

    /**
     * 每个测试方法执行前，初始化 Mock 注解并构建 MockMvc
     */
    @Before
    public void setUp() {
        MockitoAnnotations.initMocks(this);
        mockMvc = MockMvcBuilders.standaloneSetup(courseController).build();
    }

    // =========================================================================
    // 1. courseList() - GET /course/course_list
    // =========================================================================

    /**
     * 测试 courseList()：访问 GET /course/course_list
     * 预期：返回逻辑视图名 "course/courseList"，HTTP 200
     */
    @Test
    public void testCourseList_returnsViewName() throws Exception {
        mockMvc.perform(get("/course/course_list"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(view().name("course/courseList"));
    }

    // =========================================================================
    // 2. getClazzList() - POST /course/getCourseList
    // =========================================================================

    /**
     * 测试 getClazzList()：普通分页查询（无 from=combox）
     * 分支：name 为空、teacherid=0、from 为空
     * 预期：JSON 包含 total 和 rows 字段
     */
    @Test
    public void testGetCourseList_normalResponse() throws Exception {
        // 构造 mock 返回的分页数据
        List<Course> courseList = new ArrayList<>();
        Course c1 = new Course();
        c1.setId(1);
        c1.setName("Java程序设计");
        courseList.add(c1);

        PageBean<Course> pageBean = new PageBean<>(1, 10);
        pageBean.setDatas(courseList);
        pageBean.setTotalsize(1);

        when(courseService.queryPage(anyMap())).thenReturn(pageBean);

        mockMvc.perform(post("/course/getCourseList")
                .param("page", "1")
                .param("rows", "10")
                .param("teacherid", "0"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.total").value(1))
                .andExpect(jsonPath("$.rows").isArray());
    }

    /**
     * 测试 getClazzList()：带 name 过滤参数（name 非空分支）
     * 预期：paramMap 中包含 name 键
     */
    @Test
    public void testGetCourseList_withNameFilter() throws Exception {
        PageBean<Course> pageBean = new PageBean<>(1, 10);
        pageBean.setDatas(new ArrayList<>());
        pageBean.setTotalsize(0);

        when(courseService.queryPage(anyMap())).thenReturn(pageBean);

        mockMvc.perform(post("/course/getCourseList")
                .param("page", "1")
                .param("rows", "10")
                .param("name", "数学")
                .param("teacherid", "0"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.total").value(0));
    }

    /**
     * 测试 getClazzList()：带 teacherid 非 0 过滤（teacherId 分支）
     * 预期：正常返回 rows
     */
    @Test
    public void testGetCourseList_withTeacherIdFilter() throws Exception {
        PageBean<Course> pageBean = new PageBean<>(1, 10);
        pageBean.setDatas(new ArrayList<>());
        pageBean.setTotalsize(0);

        when(courseService.queryPage(anyMap())).thenReturn(pageBean);

        mockMvc.perform(post("/course/getCourseList")
                .param("page", "1")
                .param("rows", "10")
                .param("teacherid", "5"))
                .andDo(print())
                .andExpect(status().isOk());
    }

    /**
     * 测试 getClazzList()：from=combox 分支
     * 预期：直接返回 Course 列表数组，而非包含 total/rows 的对象
     */
    @Test
    public void testGetCourseList_comboxResponse() throws Exception {
        List<Course> courseList = new ArrayList<>();
        Course c1 = new Course();
        c1.setId(2);
        c1.setName("数据结构");
        courseList.add(c1);

        PageBean<Course> pageBean = new PageBean<>(1, 100);
        pageBean.setDatas(courseList);
        pageBean.setTotalsize(1);

        when(courseService.queryPage(anyMap())).thenReturn(pageBean);

        mockMvc.perform(post("/course/getCourseList")
                .param("page", "1")
                .param("rows", "100")
                .param("teacherid", "0")
                .param("from", "combox"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$[0].name").value("数据结构"));
    }

    /**
     * 测试 getClazzList()：from 非空但不等于 "combox" 的分支
     * 覆盖 from 非空 && from != "combox" → 走 else 分支返回 total+rows 的场景
     * 这是 "!StringUtils.isEmpty(from) && from.equals("combox")" 中
     * 第二个条件为 false 时的路径，也是 Branch 100% 所缺少的最后一个分支
     * 预期：返回包含 total 和 rows 的 JSON 对象
     */
    @Test
    public void testGetCourseList_fromNotCombox() throws Exception {
        List<Course> courseList = new ArrayList<>();
        Course c1 = new Course();
        c1.setId(3);
        c1.setName("操作系统");
        courseList.add(c1);

        PageBean<Course> pageBean = new PageBean<>(1, 10);
        pageBean.setDatas(courseList);
        pageBean.setTotalsize(1);

        when(courseService.queryPage(anyMap())).thenReturn(pageBean);

        // from 传 "list"（非空，但不是 "combox"），走 else 分支
        mockMvc.perform(post("/course/getCourseList")
                .param("page", "1")
                .param("rows", "10")
                .param("teacherid", "0")
                .param("from", "list"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.total").value(1))
                .andExpect(jsonPath("$.rows").isArray());
    }

    // =========================================================================
    // 3. addCourse() - POST /course/addCourse
    // =========================================================================

    /**
     * 测试 addCourse()：添加成功（count > 0）
     * 预期：success=true，message="添加成功"
     */
    @Test
    public void testAddCourse_success() throws Exception {
        when(courseService.addCourse(any(Course.class))).thenReturn(1);

        mockMvc.perform(post("/course/addCourse")
                .param("name", "软件工程")
                .param("teacherId", "3")
                .param("courseDate", "周三")
                .param("maxNum", "60")
                .param("info", "软件工程课程简介"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(true))
                .andExpect(jsonPath("$.message").value("添加成功"));
    }

    /**
     * 测试 addCourse()：添加失败（count <= 0）
     * 预期：success=false，message="添加失败"
     */
    @Test
    public void testAddCourse_fail() throws Exception {
        when(courseService.addCourse(any(Course.class))).thenReturn(0);

        mockMvc.perform(post("/course/addCourse")
                .param("name", "失败课程"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("添加失败"));
    }

    /**
     * 测试 addCourse()：Service 抛出异常（异常分支）
     * 预期：捕获异常后返回 success=false，message="添加失败"
     */
    @Test
    public void testAddCourse_exception() throws Exception {
        when(courseService.addCourse(any(Course.class)))
                .thenThrow(new RuntimeException("DB connection error"));

        mockMvc.perform(post("/course/addCourse")
                .param("name", "异常课程"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("添加失败"));
    }

    // =========================================================================
    // 4. editCourse() - POST /course/editCourse
    // =========================================================================

    /**
     * 测试 editCourse()：修改成功（count > 0）
     * 预期：success=true，message="修改成功"
     */
    @Test
    public void testEditCourse_success() throws Exception {
        when(courseService.editCourse(any(Course.class))).thenReturn(1);

        mockMvc.perform(post("/course/editCourse")
                .param("id", "1")
                .param("name", "修改后课程名")
                .param("teacherId", "2"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(true))
                .andExpect(jsonPath("$.message").value("修改成功"));
    }

    /**
     * 测试 editCourse()：修改失败（count <= 0）
     * 预期：success=false，message="修改失败"
     */
    @Test
    public void testEditCourse_fail() throws Exception {
        when(courseService.editCourse(any(Course.class))).thenReturn(0);

        mockMvc.perform(post("/course/editCourse")
                .param("id", "999"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("修改失败"));
    }

    /**
     * 测试 editCourse()：Service 抛出异常（异常分支）
     * 预期：success=false，message="修改失败"
     */
    @Test
    public void testEditCourse_exception() throws Exception {
        when(courseService.editCourse(any(Course.class)))
                .thenThrow(new RuntimeException("update failed"));

        mockMvc.perform(post("/course/editCourse")
                .param("id", "1"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("修改失败"));
    }

    // =========================================================================
    // 5. deleteCourse() - POST /course/deleteCourse
    // =========================================================================

    /**
     * 测试 deleteCourse()：删除成功（count > 0）
     * 预期：success=true，message="删除成功"
     */
    @Test
    public void testDeleteCourse_success() throws Exception {
        when(courseService.deleteCourse(anyList())).thenReturn(1);

        mockMvc.perform(post("/course/deleteCourse")
                .param("ids", "1", "2"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(true))
                .andExpect(jsonPath("$.message").value("删除成功"));
    }

    /**
     * 测试 deleteCourse()：删除失败（count <= 0）
     * 预期：success=false，message="删除失败"
     */
    @Test
    public void testDeleteCourse_fail() throws Exception {
        when(courseService.deleteCourse(anyList())).thenReturn(0);

        mockMvc.perform(post("/course/deleteCourse")
                .param("ids", "999"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("删除失败"));
    }

    /**
     * 测试 deleteCourse()：Service 抛出异常（异常分支，如外键约束）
     * 预期：success=false，message="删除失败,该班级存在老师或学生"
     */
    @Test
    public void testDeleteCourse_exception() throws Exception {
        when(courseService.deleteCourse(anyList()))
                .thenThrow(new RuntimeException("foreign key constraint"));

        mockMvc.perform(post("/course/deleteCourse")
                .param("ids", "1"))
                .andDo(print())
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success").value(false))
                .andExpect(jsonPath("$.message").value("删除失败,该班级存在老师或学生"));
    }
}
